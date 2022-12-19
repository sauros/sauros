#include "processor.hpp"

#include <iostream>
#include <sstream>

namespace sauros {

void processor_c::cell_to_string(std::string &out, cell_c &cell,
                                 std::shared_ptr<environment_c> env,
                                 bool show_space) {
   switch (cell.type) {
   case cell_type_e::DOUBLE:
      [[fallthrough]];
   case cell_type_e::STRING:
      [[fallthrough]];
   case cell_type_e::INTEGER:
      out += cell.data;
      if (show_space) {
         out += " ";
      }
      break;
   case cell_type_e::SYMBOL: {
      auto data = process_cell(cell, env);
      if (data.has_value()) {
         cell_to_string(out, *data, env);
      }
      break;
   }
   case cell_type_e::LIST: {

      out += std::string("[");

      for (auto &cell : cell.list) {
         cell_to_string(out, cell, env, true);
      }
      if (cell.list.size()) {
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
   case cell_type_e::OBJECT: {
      out += "<object>";
      break;
   }
   }
}

void processor_c::quote_cell(std::string &out, cell_c &cell,
                                 std::shared_ptr<environment_c> env) {
   switch (cell.type) {
   case cell_type_e::DOUBLE:
      [[fallthrough]];
   case cell_type_e::INTEGER:
      [[fallthrough]];
   case cell_type_e::SYMBOL: {
      out += cell.data;
      out += " ";
      break;
   }
   case cell_type_e::STRING:
      out += "\"" + cell.data + "\" ";
      break;
   case cell_type_e::LIST: {

      out += std::string("[ ");

      for (auto &cell : cell.list) {
         quote_cell(out, cell, env);
      }
      out += std::string("] ");
      break;
   }
   case cell_type_e::LAMBDA: {

      auto &cells = cell.list;

      auto lambda_name = cells[0].data;

      out += lambda_name + "[ ";

      auto target_lambda =
         env->find(cells[0].data, cells[0].location)->get(cells[0].data);

      for (auto param = target_lambda.list[0].list.begin() + 1; 
               param != target_lambda.list[0].list.end(); 
               ++param) {
          out += (*param).data + " ";
      }

      out += "] ";

      out += std::string("[");

      for (auto &cell : target_lambda.list[1].list) {
         quote_cell(out, cell, env);
      }
      out += std::string("] ");
      break;
   }
   case cell_type_e::OBJECT: {
      out = "<OBJECT> :: NOT YET DONE";
      break;
   }
   }
}

processor_c::processor_c() { populate_standard_builtins(); }

std::optional<cell_c>
processor_c::process_list(std::vector<cell_c> &cells,
                          std::shared_ptr<environment_c> env) {
   if (cells.empty()) {
      return {};
   }

   auto suspect_cell = cells[0];

   switch (suspect_cell.type) {
   case cell_type_e::SYMBOL: {

      auto cell = process_cell(suspect_cell, env);

      if (!cell.has_value()) {
         throw runtime_exception_c("Unknown symbol: " + suspect_cell.data,
                                   suspect_cell.location);
      }

      // Check if its a lambda first
      if ((*cell).type == cell_type_e::LAMBDA) {
         return process_lambda((*cell), cells, env);
      }

      // If its not then it might be a proc
      if ((*cell).proc) {
         return (*cell).proc(cells, env);
      }

      // Otherwise return whatever we got
      return {*cell};
   }

   case cell_type_e::LIST:
      return process_list(suspect_cell.list, env);

   case cell_type_e::DOUBLE:
      [[fallthrough]];
   case cell_type_e::STRING:
      [[fallthrough]];
   case cell_type_e::INTEGER:
      return process_cell(suspect_cell, env);
      break;

   case cell_type_e::LAMBDA:
      break;

   case cell_type_e::OBJECT: {

      // TODO:
      //    Considering makeing a map of cells for each defined object that
      //    containes a proc cell that loads the initially defined object cell
      //    and clones it into the environment under a new name. Then, we can
      //    use the nifty accessor.list.code to drill into that new object's
      //    contained environment and members
      //

      break;
   }

   default:
      break;
   }

   throw runtime_exception_c("Unknown cell type", cells[0].location);
}

std::optional<cell_c>
processor_c::process_cell(cell_c &cell, std::shared_ptr<environment_c> env) {

   switch (cell.type) {
   case cell_type_e::SYMBOL: {

      // Check if its a dot accessor
      //
      if (cell.data.find('.') != std::string::npos) {
         std::string accessor;
         std::vector<std::string> accessor_list;
         std::stringstream source(cell.data);
         while(std::getline(source, accessor, '.')) {
            accessor_list.push_back(accessor);
         }
         // Each item up-to and not including the last item should be an object
         // the last member should be something within the object that we are 
         // trying to access
         return access_object_member(accessor_list);
      }

      // Check the built ins to see if its a process to exec
      //
      if (_builtins.find(cell.data) != _builtins.end()) {
         return _builtins[cell.data];
      }

      // If not built in maybe it is in the environment
      //
      auto env_with_data = env->find(cell.data, cell.location);
      auto r = env_with_data->get(cell.data);
      return {r};
   }

   case cell_type_e::LIST:
      return process_list(cell.list, env);

   case cell_type_e::DOUBLE:
      [[fallthrough]];
   case cell_type_e::STRING:
      [[fallthrough]];
   case cell_type_e::INTEGER:
      [[fallthrough]];
   case cell_type_e::OBJECT:
      return cell;

   case cell_type_e::LAMBDA: {
      return process_list(cell.list, env);
   }

   default:
      break;
   }
   return {};
}

std::optional<cell_c>
processor_c::process_lambda(cell_c &cell, std::vector<cell_c> &cells,
                            std::shared_ptr<environment_c> env) {
   // Load cells[0]
   // everything after is a parameter
   // create an environment, pass the data in as the variable
   // that they will be expected as, then call

   auto target_lambda =
       env->find(cells[0].data, cells[0].location)->get(cells[0].data);

   if (target_lambda.list[0].list.size() != cells.size() - 1) {
      throw runtime_exception_c(
          "Parameters mismatch for lambda `" + cells[0].data +
              "` :: Expected " +
              std::to_string(target_lambda.list[0].list.size()) +
              " parameters, got " + std::to_string(cells.size() - 1),
          cells[0].location);
   }

   std::vector<cell_c> exps;
   for (auto param = cells.begin() + 1; param != cells.end(); ++param) {

      auto evaluated = process_cell((*param), env);
      if (!evaluated.has_value()) {
         throw runtime_exception_c(
             "Nothing returned evaluating parameter for lambda",
             cells[0].location);
      }

      exps.push_back(std::move((*evaluated)));
   }

   // Create the lambda cell
   cell_c lambda_cell;
   lambda_cell.data = cells[0].data;
   lambda_cell.type = cell.type;
   lambda_cell.list = target_lambda.list[1].list;

   // Create the lambda env
   auto lambda_env = std::make_shared<environment_c>(
       environment_c(target_lambda.list[0].list, exps, env));

   return process_cell(lambda_cell, lambda_env);
}


std::optional<cell_c>
processor_c::access_object_member(std::vector<std::string> &accessors) {

   std::cout << "Access object member > ";
   for(auto &el : accessors) {
      std::cout << el << "->";
   }
   std::cout << std::endl;

   return {};
}

} // namespace sauros