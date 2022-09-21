#include "processor.hpp"

#include <iostream>

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
   }
}

processor_c::processor_c() { populate_standard_builtins(); }

std::optional<cell_c> processor_c::process(cell_c &cell,
                                           std::shared_ptr<environment_c> env) {
   if (cell.list.empty()) {
      return {};
   }

   return process_list(cell.list, env);
}

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
      // Unused
      break;

   default:
      break;
   }

   throw runtime_exception_c("Unknown cell type", cells[0].location);
}

std::optional<cell_c>
processor_c::process_cell(cell_c &cell, std::shared_ptr<environment_c> env) {

   switch (cell.type) {
   case cell_type_e::SYMBOL: {

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

   auto target_lambda = env->find(cells[0].data, cells[0].location)->get(cells[0].data);

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

} // namespace sauros