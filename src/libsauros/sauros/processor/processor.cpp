#include "processor.hpp"

#include <iostream>
#include <sstream>

namespace sauros {

void processor_c::cell_to_string(std::string &out, cell_ptr cell,
                                 std::shared_ptr<environment_c> env,
                                 bool show_space) {
   switch (cell->type) {
   case cell_type_e::DOUBLE:
      [[fallthrough]];
   case cell_type_e::STRING:
      [[fallthrough]];
   case cell_type_e::INTEGER:
      out += cell->data;
      if (show_space) {
         out += " ";
      }
      break;
   case cell_type_e::ENCODED_SYMBOL:
      [[fallthrough]];
   case cell_type_e::SYMBOL: {
      auto data = process_cell(cell, env);
      cell_to_string(out, data, env);
      break;
   }
   case cell_type_e::LIST: {

      out += std::string("[");

      for (auto &cell : cell->list) {
         cell_to_string(out, cell, env, true);
      }
      if (cell->list.size()) {
         out.pop_back();
      }
      out += std::string("]");
      if (show_space) {
         out += " ";
      }
      break;
   }
   case cell_type_e::LAMBDA: {
      out += "<lambda>";
      break;
   }
   case cell_type_e::BOX: {
      out += "<box>";
      break;
   }
   }
}

void processor_c::quote_cell(std::string &out, cell_ptr cell,
                             std::shared_ptr<environment_c> env) {
   switch (cell->type) {
   case cell_type_e::DOUBLE:
      [[fallthrough]];
   case cell_type_e::INTEGER:
      [[fallthrough]];
   case cell_type_e::ENCODED_SYMBOL:
      [[fallthrough]];
   case cell_type_e::SYMBOL: {
      out += cell->data;
      out += " ";
      break;
   }
   case cell_type_e::STRING:
      out += "\"" + cell->data + "\" ";
      break;
   case cell_type_e::LIST: {

      out += std::string("[ ");

      for (auto &cell : cell->list) {
         quote_cell(out, cell, env);
      }
      out += std::string("] ");
      break;
   }
   case cell_type_e::LAMBDA: {

      auto &cells = cell->list;

      auto lambda_name = cells[0]->data;

      out += lambda_name + "[ ";

      auto target_lambda =
          env->find(cells[0]->data, cells[0]->location)->get(cells[0]->data);

      for (auto param = target_lambda->list[0]->list.begin() + 1;
           param != target_lambda->list[0]->list.end(); ++param) {
         out += (*param)->data + " ";
      }

      out += "] ";

      out += std::string("[");

      for (auto &cell : target_lambda->list[1]->list) {
         quote_cell(out, cell, env);
      }
      out += std::string("] ");
      break;
   }
   case cell_type_e::BOX: {
      break;
   }
   }
}

processor_c::processor_c() { populate_standard_builtins(); }

cell_ptr processor_c::process_list(cells_t &cells,
                                   std::shared_ptr<environment_c> env) {
   if (cells.empty()) {
      return std::make_shared<cell_c>(CELL_NIL);
   }

   auto suspect_cell = cells[0];

   switch (suspect_cell->type) {
   case cell_type_e::ENCODED_SYMBOL:
      [[fallthrough]];
   case cell_type_e::SYMBOL: {

      auto cell = process_cell(suspect_cell, env);
      // Check if its a lambda first
      if (cell->type == cell_type_e::LAMBDA) {
         return process_lambda(cell, cells, env);
      }

      // If its not then it might be a proc
      if (cell->proc) {
         return cell->proc(cells, env);
      }

      // Otherwise return whatever we got
      return cell;
   }

   case cell_type_e::LIST:
      return process_list(suspect_cell->list, env);

   case cell_type_e::DOUBLE:
      [[fallthrough]];
   case cell_type_e::STRING:
      [[fallthrough]];
   case cell_type_e::INTEGER:
      return process_cell(suspect_cell, env);
      break;
   case cell_type_e::LAMBDA:
      [[fallthrough]];
   case cell_type_e::BOX:
      break;

   default:
      break;
   }
   throw runtime_exception_c("Unknown cell type", cells[0]->location);
}

std::vector<std::string>
processor_c::retrieve_accessors(const std::string &value) {
   std::string accessor;
   std::vector<std::string> accessor_list;
   std::stringstream source(value);
   while (std::getline(source, accessor, '.')) {
      accessor_list.push_back(accessor);
   }
   return accessor_list;
}

cell_ptr processor_c::process_cell(cell_ptr cell,
                                   std::shared_ptr<environment_c> env) {

   switch (cell->type) {
   case cell_type_e::SYMBOL: {

      // Check if its a dot accessor
      //
      if (cell->data.find('.') != std::string::npos) {
         // Each item up-to and not including the last item should be n box
         // the last member should be something within the box that we are
         // trying to access
         return access_box_member(cell, env);
      }

      // If not built in maybe it is in the environment
      //
      auto env_with_data = env->find(cell->data, cell->location);
      auto r = env_with_data->get(cell->data);
      return {r};
   }

   case cell_type_e::ENCODED_SYMBOL: {
      if (cell->builtin_encoding == BUILTIN_DEFAULT_VAL ||
          cell->builtin_encoding >= BUILTIN_ENTRY_COUNT) {
         throw runtime_exception_c("Invalid encoded symbol for : " + cell->data,
                                   cell->location);
         return {};
      }

      // Direct access - no more mapping
      return _builtins[cell->builtin_encoding];
   }

   case cell_type_e::LIST:
      return process_list(cell->list, env);
   case cell_type_e::DOUBLE:
      [[fallthrough]];
   case cell_type_e::STRING:
      [[fallthrough]];
   case cell_type_e::INTEGER:
      return cell;
   case cell_type_e::LAMBDA: {
      return process_list(cell->list, env);
   case cell_type_e::BOX:
      break;
   }

   default:
      break;
   }
   return {};
}

cell_ptr processor_c::process_lambda(cell_ptr cell, cells_t &cells,
                                     std::shared_ptr<environment_c> env) {
   cells_t exps;
   for (auto param = cells.begin() + 1; param != cells.end(); ++param) {

      auto evaluated = process_cell((*param), env);
      exps.push_back(std::move(evaluated));
   }

   if (cell->list[0]->list.size() != exps.size()) {
      throw runtime_exception_c(
          "Invalid number of paramters given to lambda: " + cells[0]->data +
              ". " + std::to_string(exps.size()) + " parameters given, but " +
              std::to_string(cell->list[0]->list.size()) + " were expected.",
          cells[0]->location);
   }

   // Create the lambda cell
   cell_ptr lambda_cell = std::make_shared<cell_c>();
   lambda_cell->data = cells[0]->data;
   lambda_cell->type = cell->type;
   lambda_cell->list = cell->list[1]->list;

   // Create the lambda env
   auto lambda_env = std::make_shared<environment_c>(
       environment_c(cell->list[0]->list, exps, env));

   return process_cell(lambda_cell, lambda_env);
}

cell_ptr processor_c::access_box_member(cell_ptr cell,
                                        std::shared_ptr<environment_c> &env) {

   auto accessors = retrieve_accessors(cell->data);

   if (accessors.size() <= 1) {
      throw runtime_exception_c("Malformed accessor", cell->location);
   }

   cell_ptr result;
   std::shared_ptr<environment_c> moving_env = env;
   for (std::size_t i = 0; i < accessors.size(); i++) {

      // Get the item from the accessor
      auto containing_env = moving_env->find(accessors[i], cell->location);
      result = containing_env->get(accessors[i]);

      // Check if we need to move the environment "in" to the next box
      if (result->type == cell_type_e::BOX) {
         moving_env = result->box_env;
      }
   }

   return {result};
}

} // namespace sauros