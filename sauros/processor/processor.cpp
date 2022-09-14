#include "processor.hpp"

#include <iostream>

namespace sauros {

void processor_c::cell_to_string(std::string& out, cell_c& cell, std::shared_ptr<environment_c> env, bool show_space) {

   switch(cell.type) {
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

         for (auto& cell : cell.list) {
            cell_to_string(out, cell, env, true);
         }
         if (cell.list.size()){
            out.pop_back();
         }
         out += std::string("]");
         break;
      }
      case cell_type_e::LAMBDA:
         out += std::string("<lambda>");
         break;
   }
}

processor_c::processor_c() {
   populate_builtins();
}

processor_c::result_s processor_c::process(cell_c& cell, std::shared_ptr<environment_c> env) {

   result_s result;

   result.returned_value = process_list(cell.list, env);

   return result;
}

std::optional<cell_c> processor_c::process_cell(cell_c& cell, std::shared_ptr<environment_c> env) {

   switch (cell.type) {
      case cell_type_e::SYMBOL: {

         // Check the built ins to see if its a process to exec
         //
         if (_builtins.find(cell.data) != _builtins.end()) {
            return _builtins[cell.data];
         }

         // If not built in maybe it is in the environment
         //
         auto env_with_data = env->find(cell.data);
         auto r = env_with_data->get(cell.data);
         return {r};
       break;
      }

      case cell_type_e::LIST:
         return process_list(cell.list, env);
      break;

      case cell_type_e::DOUBLE:
         [[fallthrough]];
      case cell_type_e::STRING:
         [[fallthrough]];
      case cell_type_e::INTEGER:
         return cell;
      break;

      case cell_type_e::LAMBDA: {
         return process_list(cell.list, env);
      }

      default:
         break;
   }
   return {};
}

std::optional<cell_c>  processor_c::process_list(std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) {

   // Check common functions for first element
   //
   auto suspect_cell = cells[0];

   switch (suspect_cell.type) {
      case cell_type_e::SYMBOL: {

         // Right now only callable things can be the first item at the front of the list
         // --- and they will all be process cells 
         auto cell = process_cell(suspect_cell, env);

         if (!cell.has_value()) {
            throw runtime_exception_c("Unknown symbol: " + suspect_cell.data, suspect_cell.location);
         }

         // Check if its a lambda first
         if ((*cell).type == cell_type_e::LAMBDA) {
            // Load cells[0] 
            // everything after is a parameter
            // create an environment, pass the data in as the variable
            // that they will be expected as, then call

            auto target_lambda = env->find(cells[0].data)->get(cells[0].data);

            std::vector<cell_c> exps;
            for(auto param = cells.begin()+1; param != cells.end(); ++param) {

               auto evaluated = process_cell((*param), env);
               if (!evaluated.has_value()) {
                  throw runtime_exception_c("Nothing returned evaluating parameter for lambda", cells[0].location);
               }

               exps.push_back(std::move((*evaluated)));
            }

            // Create the lambda cell
            cell_c lambda_cell;
            lambda_cell.type = (*cell).type;
            lambda_cell.list = target_lambda.list[1].list;

            // Create the lambda env
            auto lambda_env = std::make_shared<environment_c>(environment_c(
               target_lambda.list[0].list,
               exps,
               env));

            return process_cell(lambda_cell, lambda_env);
         }

         // If its not then it has to be a proc
         if ((*cell).proc) {
            return (*cell).proc(cells, env);
         }

         return {*cell};
      }

      case cell_type_e::LIST:

         std::cout << "Handle list" << suspect_cell.data << std::endl;

         return process_list(suspect_cell.list, env);
      break;
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


void processor_c::populate_builtins() {

   _builtins["var"] = cell_c(
      [this](std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
         auto& variable_name = cells[1].data;
         auto value = process_cell(cells[2], env);

         if ((*value).type == cell_type_e::SYMBOL) {
            throw runtime_exception_c("Expected list or datum value", cells[2].location);
         }

         if (!value.has_value()) {
            throw runtime_exception_c("Unable to locate value for assignment", cells[2].location);
         }

         env->set(variable_name, (*value));
         return {env->get(variable_name)};
      });

   _builtins["put"] = cell_c(
      [this](std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
         for(auto i = cells.begin() + 1; i != cells.end(); ++i) {

            auto item = process_cell((*i), env);

            if (!item.has_value()) {
               throw runtime_exception_c("Unknown operand given to 'put'", cells[1].location);
            }

            std::string stringed;
            cell_to_string(stringed, (*item), env, false);
            std::cout << stringed;

         }
         std::cout << std::endl;

         return {};
      });

   _builtins["lambda"] = cell_c(
      [this](std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
         // First item following lambda must be a list of parameters
         std::vector<cell_c> body(cells.begin()+1, cells.end());
         cell_c lambda(body);
         lambda.type = cell_type_e::LAMBDA;
         lambda.data = cells[1].data;
         return {lambda};
      });

   _builtins["block"] = cell_c(
      [this](std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
         // First item following lambda must be a list of parameters

         for (size_t i = 1; i < cells.size() - 1; i++) {
            process_cell(cells[i], env);
         }
         return process_cell(cells[cells.size()-1], env);
      });

   _builtins["set"] = cell_c(
      [this](std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
         
         if (cells.size() != 3) {
            throw runtime_exception_c("set command expects 2 parameters, but" 
               + std::to_string(cells.size()-1) 
               + " were given", cells[0].location);
         }

         auto& variable_name = cells[1].data;

         // If this isn't found it will throw :)
         { auto _ = env->find(variable_name); }

         auto value = process_cell(cells[2], env);

         if ((*value).type == cell_type_e::SYMBOL) {
            throw runtime_exception_c("Expected list or datum value", cells[2].location);
         }

         if (!value.has_value()) {
            throw runtime_exception_c("Unable to locate value for assignment", cells[2].location);
         }

         env->set(variable_name, (*value));
         return {env->get(variable_name)};
      });

   _builtins["list"] = cell_c(
      [this](std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
         // First item following lambda must be a list of parameters
         std::vector<cell_c> body;

         for (auto i = cells.begin()+1; i != cells.end(); ++i) {
            body.push_back((*process_cell(*i, env)));
         }

         cell_c list(body);
         list.type = cell_type_e::LIST;
         list.data = "<list>";
         return {list};
      });

   _builtins["len"] = cell_c(
      [this](std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
         
         if (cells.size() != 2) {
            throw runtime_exception_c("len command expects 2 parameters, but " 
               + std::to_string(cells.size()-1) 
               + " were given", cells[0].location);
         }

         return { cell_c(cell_type_e::INTEGER, std::to_string((*process_cell(cells[1], env)).list.size()), cells[1].location) };
      });

   _builtins["if"] = cell_c(
      [this](std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

         if (cells.size() !=  4) {
            throw runtime_exception_c("if command expects 4 parameters, but " 
               + std::to_string(cells.size()-1) 
               + " were given", cells[0].location);
         }

         auto eval = process_cell(cells[1], env);
         if (!eval.has_value()) {
            throw runtime_exception_c("Evaluated cell contained no value to evaluate", cells[1].location);
         }

         bool is_true{false};
         switch ((*eval).type) {
            case cell_type_e::STRING: is_true = (!(*eval).data.empty()); break;
            case cell_type_e::LIST: is_true = (!(*eval).list.empty()); break;
            case cell_type_e::LAMBDA: is_true = true; break;
            case cell_type_e::DOUBLE:
               [[fallthrough]];
            case cell_type_e::INTEGER: {
               try {
                  auto v = std::stod((*eval).data);
                  is_true = (v > 0.0);
               }
               catch (const std::invalid_argument&) {
                  throw runtime_exception_c("Invalid data type given for operand", cells[0].location);
               }
               catch (const std::out_of_range&) {
                  throw runtime_exception_c("Item caused out of range exception", cells[0].location);
               }
               break;
            }
         }

         return (is_true) ? process_cell(cells[2], env) : process_cell(cells[3], env);
      });

   _builtins["seq"] = cell_c(
      [this](std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

         if (cells.size() !=  3) {
            throw runtime_exception_c("eq command expects 3 parameters, but " 
               + std::to_string(cells.size()-1) 
               + " were given", cells[0].location);
         }

         auto eval = process_cell(cells[1], env);
         if (!eval.has_value()) {
            return {CELL_FALSE};
         }

         return cell_c(
            cell_type_e::INTEGER, std::to_string((cells[1].data == cells[2].data)), cells[0].location
         );
      });

   _builtins["sneq"] = cell_c(
      [this](std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

         if (cells.size() !=  3) {
            throw runtime_exception_c("eq command expects 3 parameters, but " 
               + std::to_string(cells.size()-1) 
               + " were given", cells[0].location);
         }

         auto eval = process_cell(cells[1], env);
         if (!eval.has_value()) {
            return {CELL_FALSE};
         }

         return cell_c(
            cell_type_e::INTEGER, std::to_string((cells[1].data != cells[2].data)), cells[0].location
         );
      });

   _builtins["<"] = cell_c(
      [this](std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
         return {perform_arithmetic("<", cells, [](double lhs, double rhs) -> double {
            return lhs < rhs;
         },
         env)};
      });

   _builtins["<="] = cell_c(
      [this](std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
         return {perform_arithmetic("<=", cells, [](double lhs, double rhs) -> double {
            return lhs <= rhs;
         },
         env)};
      });

   _builtins[">"] = cell_c(
      [this](std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
         return {perform_arithmetic(">", cells, [](double lhs, double rhs) -> double {
            return lhs > rhs;
         },
         env)};
      });

   _builtins[">="] = cell_c(
      [this](std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
         return {perform_arithmetic(">=", cells, [](double lhs, double rhs) -> double {
            return lhs >= rhs;
         },
         env)};
      });

   _builtins["=="] = cell_c(
      [this](std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
         return {perform_arithmetic("==", cells, [](double lhs, double rhs) -> double {
            return lhs == rhs;
         },
         env)};
      });

   _builtins["!="] = cell_c(
      [this](std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
         return {perform_arithmetic("!=", cells, [](double lhs, double rhs) -> double {
            return lhs != rhs;
         },
         env)};
      });

   _builtins["+"] = cell_c(
      [this](std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
         return {perform_arithmetic("+", cells, [](double lhs, double rhs) -> double {
            return lhs + rhs;
         },
         env)};
      });

   _builtins["-"] = cell_c(
      [this](std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
         return {perform_arithmetic("-", cells, [](double lhs, double rhs) -> double {
            return lhs - rhs;
         },
         env)};
      });

   _builtins["/"] = cell_c(
      [this](std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
         return {perform_arithmetic("/", cells, [=](double lhs, double rhs) -> double {

            if (rhs == 0.0) {
               return 0.0;
            }

            return lhs / rhs;
         }, 
         env,
         true)};
      });

   _builtins["*"] = cell_c(
      [this](std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
         return {perform_arithmetic("*", cells, [](double lhs, double rhs) -> double {
            return lhs * rhs;
         },
         env)};
      });

   _builtins["%"] = cell_c(
      [this](std::vector<cell_c>& cells, std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
         return {perform_arithmetic("%", cells, [](double lhs, double rhs) -> double {
            return static_cast<int64_t>(lhs) % static_cast<int64_t>(rhs);
         },
         env)};
      });
}

cell_c processor_c::perform_arithmetic(std::string op, std::vector<cell_c>& cells, std::function<double(double, double)> fn, std::shared_ptr<environment_c> env, bool force_double) {

   if (cells.size() < 3) {
      throw runtime_exception_c("Expected a list size of at least 3 items", cells[0].location);
   }

   double result = 0.0;
   bool store_as_double {false};

   auto first_cell_value = process_cell(cells[1], env);
   if (!first_cell_value.has_value()) {
      throw runtime_exception_c("Unknown operand given to '" + op + "'", cells[1].location);
   }
   try {
      result = std::stod((*first_cell_value).data);
   }
   catch (const std::invalid_argument&) {
      throw runtime_exception_c("Invalid data type given for operand", cells[0].location);
   }
   catch (const std::out_of_range&) {
      throw runtime_exception_c("Item caused out of range exception", cells[0].location);
   }

   for(auto i = cells.begin() + 2; i != cells.end(); ++i) {

      auto cell_value = process_cell((*i), env);
      if (!cell_value) {
         throw runtime_exception_c("Unknown operand given to '" + op + "'", cells[0].location);
      }

      if ((*cell_value).type == cell_type_e::DOUBLE) {
         store_as_double = true;
      } else if ((*cell_value).type != cell_type_e::INTEGER) {
         throw runtime_exception_c("Invalid type for operand", (*i).location);
      }

      try {
         result = fn(result, std::stod((*cell_value).data));
      }
      catch (const std::invalid_argument&) {
         throw runtime_exception_c("Invalid data type given for operand", cells[0].location);
      }
      catch (const std::out_of_range&) {
         throw runtime_exception_c("Item caused out of range exception", cells[0].location);
      }
   }

   if (force_double || store_as_double) {
      return cell_c(cell_type_e::DOUBLE, std::to_string(result), cells[0].location);
   } else {
      return cell_c(
         cell_type_e::INTEGER, 
         std::to_string(static_cast<int64_t>(result)), 
         cells[0].location
      );
   }
}

} // namespace sauros