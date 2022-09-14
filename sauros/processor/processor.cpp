#include "processor.hpp"

#include <iostream>

namespace sauros {

void show_list(std::vector<sauros::cell_c>& list) {

   std::cout << "[ ";
   for (auto& item : list) {

      switch (item.type) {
         case sauros::cell_type_e::DOUBLE:
         case sauros::cell_type_e::INTEGER:
         case sauros::cell_type_e::STRING:
         case sauros::cell_type_e::SYMBOL:
            std::cout << item.data << " ";
            break;
         case sauros::cell_type_e::LIST:
            show_list(item.list);
            break;
      }

   }
   std::cout << "] ";
}

processor_c::processor_c(std::shared_ptr<environment_c> global_env) : _global_env(global_env) {
   populate_builtins();
}

processor_c::result_s processor_c::process(cell_c& cell) {

   result_s result;

   //std::cout << "Processing list : " << std::endl;
   show_list(cell.list);
   std::cout << std::endl;

   for (auto cell : cell.list) {
      cell.env = std::make_shared<environment_c>(_global_env);
   }

   process_list(cell.list);

   return result;
}

void processor_c::populate_builtins() {

   _builtins["var"] = cell_c(std::bind(&processor_c::fn_define_variable, this, std::placeholders::_1), _global_env);
   _builtins["put"] = cell_c(std::bind(&processor_c::fn_put, this, std::placeholders::_1), _global_env);
   _builtins["lambda"] = cell_c(std::bind(&processor_c::fn_make_lambda, this, std::placeholders::_1), _global_env);
   _builtins["+"] = cell_c(std::bind(&processor_c::fn_add, this, std::placeholders::_1), _global_env);
   _builtins["-"] = cell_c(std::bind(&processor_c::fn_sub, this, std::placeholders::_1), _global_env);
   _builtins["/"] = cell_c(std::bind(&processor_c::fn_div, this, std::placeholders::_1), _global_env);
   _builtins["*"] = cell_c(std::bind(&processor_c::fn_mul, this, std::placeholders::_1), _global_env);
   _builtins["%"] = cell_c(std::bind(&processor_c::fn_mod, this, std::placeholders::_1), _global_env);
}

std::optional<cell_c> processor_c::process_cell(cell_c& cell) {

   switch (cell.type) {
      case cell_type_e::SYMBOL: {

         // Check the built ins to see if its a process to exec
         //
         if (_builtins.find(cell.data) != _builtins.end()) {
            return _builtins[cell.data];
         }

         // If not built in then it mist be in the environment
         //
         auto env_with_data = cell.env->find(cell.data);
         return {env_with_data->get(cell.data)};
       break;
      }

      case cell_type_e::LIST:
         return process_list(cell.list);
      break;

      case cell_type_e::DOUBLE:
         [[fallthrough]];
      case cell_type_e::STRING:
         [[fallthrough]];
      case cell_type_e::INTEGER:
         return cell;
      break;

      case cell_type_e::LAMBDA: {
         std::cout << "MOOT\n";
         
            std::cout << "LAMBDA DATA TO USE: \n";
            show_list(cell.list);
            std::cout << "\n";

         process_list(cell.list);

      break;
      }

      default:
         break;
   }
   return {};
}

std::optional<cell_c>  processor_c::process_list(std::vector<cell_c>& cells) {

   // Check common functions for first element
   //
   auto suspect_cell = cells[0];

   switch (suspect_cell.type) {
      case cell_type_e::SYMBOL: {

         // Right now only callable things can be the first item at the front of the list
         // --- and they will all be process cells 
         auto cell = process_cell(suspect_cell);

         if (!cell.has_value()) {
            throw runtime_exception_c("Unknown symbol: " + suspect_cell.data, suspect_cell.location);
         }

         // Check if its a lambda first
         if ((*cell).type == cell_type_e::LAMBDA) {

            std::cout << "GOT THAT L\n";


            // Create the lambda t be executed given the current sells
            // then call process_cell 


            std::cout << "DATA: \n";
            show_list(cells);
            std::cout << "\n";

            // Load cells[0] 

            // everything after is a parameter
            // need to create an environment, pass the data in as the variable
            // that they will be expected as, then call

            auto target_lambda = cells[0].env->find(cells[0].data)->get(cells[0].data);

            std::cout << "LOADED ITEM: \n";
            show_list(target_lambda.list[1].list);
            std::cout << "\n";

            std::vector<cell_c> exps;
            for(auto param = cells.begin()+1; param != cells.end(); ++param) {

               auto evaluated = process_cell(*param);
               std::cout << "EVCALSAS: " << (*evaluated).data << std::endl;
               if (!evaluated.has_value()) {
                  throw runtime_exception_c("Nothing returned evaluating parameter for lambda", cells[0].location);
               }

               exps.push_back(std::move((*evaluated)));
            }


            // Some issue with shared pointers here

         //   cells[2].env = std::shared_ptr<environment_c>(new environment_c(
         //      target_lambda.list[0].list,
         //      exps,
         //     cells[0].env));

            cell_c c;
            c.type = (*cell).type;
            c.list = target_lambda.list[1].list;
            c.env = std::shared_ptr<environment_c>(new environment_c(
               target_lambda.list[0].list,
               exps,
              cells[0].env));

            std::cout << "WAHT\n";
            return process_cell(c);
         }

         // If its not then it has to be a proc
         if (!(*cell).proc) {
            throw runtime_exception_c("Undefined proc method", suspect_cell.location);
         }

         return (*cell).proc(cells);
      }

      case cell_type_e::LIST:
         return process_list(suspect_cell.list);
      break;
      case cell_type_e::DOUBLE:
         [[fallthrough]];
      case cell_type_e::STRING:
         [[fallthrough]];
      case cell_type_e::INTEGER:
         return process_cell(suspect_cell);
      break;

      case cell_type_e::LAMBDA:
         std::cout << "LAMBDA2" << std::endl;
      break;

      default:
         break;
   }

   throw runtime_exception_c("Unknown cell type", cells[0].location);
}

std::optional<cell_c> processor_c::fn_define_variable(std::vector<cell_c>& cells) { 

   std::cout << "--------------------------\n";
   show_list(cells);
   std::cout << "\n";

   auto& variable_name = cells[1].data;
   auto value = process_cell(cells[2]);

   if ((*value).type == cell_type_e::SYMBOL) {
      throw runtime_exception_c("Expected list or datum value", cells[2].location);
   }

   if (!value.has_value()) {
      throw runtime_exception_c("Unable to locate value for assignment", cells[2].location);
   }
   
   std::cout << "Setting the variable : " << (int)(*value).type << std::endl;;

   (*value).env = std::make_shared<environment_c>(_global_env);
   _global_env->set(variable_name, (*value));
   return {};
}

std::optional<cell_c> processor_c::fn_put(std::vector<cell_c>& cells) {

   for(auto i = cells.begin() + 1; i != cells.end(); ++i) {

      auto item = process_cell((*i));

      if (!item.has_value()) {
         throw runtime_exception_c("Unknown operand given to 'put'", cells[1].location);
      }

      std::cout << (*item).data << " ";
   }
   std::cout << std::endl;

   return {};
}

std::optional<cell_c> processor_c::fn_make_lambda(std::vector<cell_c>& cells) {

   //std::cout << "-----------------------\n";
   //show_list(cells);
   //std::cout << "\n";

   // First item following lambda must be a list of parameters
   std::vector<cell_c> body(cells.begin()+1, cells.end());
   cell_c lambda(body);
   lambda.type = cell_type_e::LAMBDA;
   lambda.data = cells[1].data;
   lambda.env = std::make_shared<environment_c>(cells[0].env);
   return {lambda};
}

cell_c processor_c::perform_arithmetic(std::string op, std::vector<cell_c>& cells, std::function<double(double, double)> fn, bool force_double) {

   if (cells.size() < 3) {
      throw runtime_exception_c("Expected a list size of at least 3 items", cells[0].location);
   }

   double result = 0.0;
   bool store_as_double {false};

   auto first_cell_value = process_cell(cells[1]);
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

      auto cell_value = process_cell((*i));
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
      return cell_c(cell_type_e::DOUBLE, std::to_string(result), cells[0].location, cells[1].env);
   } else {
      return cell_c(
         cell_type_e::INTEGER, 
         std::to_string(static_cast<int64_t>(result)), 
         cells[0].location,
         cells[1].env
      );
   }
}

std::optional<cell_c> processor_c::fn_add(std::vector<cell_c>& cells) { 
   return {perform_arithmetic("+", cells, [](double lhs, double rhs) -> double {
      return lhs + rhs;
   })};
}

std::optional<cell_c> processor_c::fn_sub(std::vector<cell_c>& cells) {
   return {perform_arithmetic("-", cells, [](double lhs, double rhs) -> double {
      return lhs - rhs;
   })};
}

std::optional<cell_c> processor_c::fn_div(std::vector<cell_c>& cells) {
   return {perform_arithmetic("/", cells, [=](double lhs, double rhs) -> double {

      if (rhs == 0.0) {
         return 0.0;
      }

      return lhs / rhs;
   }, true)};
}

std::optional<cell_c> processor_c::fn_mul(std::vector<cell_c>& cells) {
   return {perform_arithmetic("*", cells, [](double lhs, double rhs) -> double {
      return lhs * rhs;
   })};
}

std::optional<cell_c> processor_c::fn_mod(std::vector<cell_c>& cells) {
   return {perform_arithmetic("%", cells, [](double lhs, double rhs) -> double {
      return static_cast<int64_t>(lhs) % static_cast<int64_t>(rhs);
   })};
}

} // namespace sauros