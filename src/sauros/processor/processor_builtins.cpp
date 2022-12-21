#include "../driver.hpp"
#include "processor.hpp"

#include <iostream>

namespace sauros {

namespace {

static inline bool eval_truthy(cell_c &cell, location_s &location) {
   switch (cell.type) {
   case cell_type_e::STRING:
      return (!cell.data.empty());
      break;
   case cell_type_e::LIST:
      return (!cell.list.empty());
      break;
   case cell_type_e::LAMBDA:
      return true;
      break;
   case cell_type_e::DOUBLE:
      [[fallthrough]];
   case cell_type_e::INTEGER: {
      try {
         auto v = std::stod(cell.data);
         return (v > 0.0);
      } catch (const std::invalid_argument &) {
         throw processor_c::runtime_exception_c(
             "Invalid data type given for operand", location);
      } catch (const std::out_of_range &) {
         throw processor_c::runtime_exception_c(
             "Item caused out of range exception", location);
      }
      break;
   }
   }
   return false;
}

} // namespace

void processor_c::populate_standard_builtins() {
   _builtins[BUILTIN_IMPORT] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() < 2) {
             throw runtime_exception_c(
                 "import command expects at least 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          file_executor_c loader(env);
          for (auto i = cells.begin() + 1; i < cells.end(); i++) {
             if ((*i).type != sauros::cell_type_e::STRING) {
                throw sauros::processor_c::runtime_exception_c(
                    "Import objects are expected to be raw strings",
                    (*i).location);
             }
             if (0 != loader.run((*i).data)) {
                throw sauros::processor_c::runtime_exception_c(
                    "Unable to load import: " + (*i).data, (*i).location);
             }
          }
          return CELL_TRUE;
       });

   _builtins[BUILTIN_EXTERN] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() < 2) {
             throw runtime_exception_c(
                 "use command expects at least 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          for (auto i = cells.begin() + 1; i < cells.end(); i++) {
             if ((*i).type != sauros::cell_type_e::STRING) {
                throw sauros::processor_c::runtime_exception_c(
                    "extern command expects parameters to be raw strings",
                    (*i).location);
             }

             load_library((*i).data, (*i).location, env);
          }
          return {sauros::CELL_TRUE};
       });

   _builtins[BUILTIN_USE] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() < 2) {
             throw runtime_exception_c(
                 "use command expects at least 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          for (auto i = cells.begin() + 1; i < cells.end(); i++) {
             if ((*i).type != sauros::cell_type_e::STRING) {
                throw sauros::processor_c::runtime_exception_c(
                    "use command expects parameters to be raw strings",
                    (*i).location);
             }

             if (!_modules.contains((*i).data)) {
                throw sauros::processor_c::runtime_exception_c("unknown module",
                                                               (*i).location);
             }

             _modules.populate_environment((*i).data, env);
          }
          return {sauros::CELL_TRUE};
       });

   _builtins[BUILTIN_EXIT] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "exit command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          std::exit(std::stoull(process_cell(cells[1], env).data));
       });

   _builtins[BUILTIN_BREAK] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 1) {
             throw runtime_exception_c(
                 "break command expects 0 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto return_value = CELL_NIL;
          return_value.stop_processing = true;
          return {return_value};
       });

   _builtins[BUILTIN_TYPE] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "type command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto target = process_cell(cells[1], env);

          return {cell_c(cell_type_e::STRING, cell_type_to_string(target.type),
                         cells[1].location)};
       });

   _builtins[BUILTIN_FRONT] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "front command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto target = process_cell(cells[1], env);

          if (!(target.type == cell_type_e::LIST)) {
             throw runtime_exception_c("Expected list parameter for front",
                                       cells[1].location);
          }

          if (target.list.empty()) {
             return {CELL_NIL};
          }

          return {target.list[0]};
       });

   _builtins[BUILTIN_BACK] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "back command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto target = process_cell(cells[1], env);

          if (!(target.type == cell_type_e::LIST)) {
             throw runtime_exception_c("Expected list parameter for back",
                                       cells[1].location);
          }

          if (target.list.empty()) {
             return {CELL_NIL};
          }

          return {*(target.list.end() - 1)};
       });

   _builtins[BUILTIN_AT] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 3) {
             throw runtime_exception_c("at command expects 2 parameters, but " +
                                           std::to_string(cells.size() - 1) +
                                           " were given",
                                       cells[0].location);
          }

          auto index = process_cell(cells[1], env);
          if (index.type != cell_type_e::INTEGER) {
             throw runtime_exception_c(
                 "at command index must me an integer type", cells[0].location);
          }

          uint64_t idx = std::stoull(index.data);

          if (cells[2].type != cell_type_e::SYMBOL) {
             throw runtime_exception_c(
                 "Second parameter of at must be a variable",
                 cells[2].location);
          }

          auto &variable_name = cells[2].data;

          // If this isn't found it will throw :)
          auto containing_env = env->find(variable_name, cells[1].location);
          cell_c &target = containing_env->get(variable_name);

          if (target.list.size() <= idx) {
             return {CELL_NIL};
          }
          return {target.list[idx]};
       });

   _builtins[BUILTIN_CLEAR] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "clear command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          if (cells[1].type != cell_type_e::SYMBOL) {
             throw runtime_exception_c(
                 "First parameter of clear must be a variable",
                 cells[1].location);
          }

          auto &variable_name = cells[1].data;

          // If this isn't found it will throw :)
          auto containing_env = env->find(variable_name, cells[1].location);
          cell_c &target = containing_env->get(variable_name);
          target.list.clear();
          return {CELL_TRUE};
       });

   _builtins[BUILTIN_POP] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "pop command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          if (cells[1].type != cell_type_e::SYMBOL) {
             throw runtime_exception_c(
                 "First parameter of pop must be a variable",
                 cells[1].location);
          }

          auto &variable_name = cells[1].data;

          // If this isn't found it will throw :)
          auto containing_env = env->find(variable_name, cells[1].location);
          cell_c &target = containing_env->get(variable_name);
          if (!target.list.empty()) {
             target.list.pop_back();
          }
          return {CELL_TRUE};
       });

   _builtins[BUILTIN_PUSH] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 3) {
             throw runtime_exception_c(
                 "push command expects 2 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          if (cells[1].type != cell_type_e::SYMBOL) {
             throw runtime_exception_c(
                 "First parameter of push must be a variable",
                 cells[1].location);
          }

          auto &variable_name = cells[1].data;

          // If this isn't found it will throw :)
          auto containing_env = env->find(variable_name, cells[1].location);
          auto value = process_cell(cells[2], env);

          if (value.type == cell_type_e::SYMBOL) {
             throw runtime_exception_c("Expected list or datum value (push)",
                                       cells[2].location);
          }

          cell_c &target = containing_env->get(variable_name);
          target.list.push_back(value);
          return {CELL_TRUE};
       });

   _builtins[BUILTIN_NOT] = cell_c([this](std::vector<cell_c> &cells,
                                          std::shared_ptr<environment_c> env)
                                       -> cell_c {
      if (cells.size() != 2) {
         throw runtime_exception_c("not command expects 1 parameters, but " +
                                       std::to_string(cells.size() - 1) +
                                       " were given",
                                   cells[0].location);
      }

      auto target = process_cell(cells[1], env);

      if (target.type != cell_type_e::INTEGER &&
          target.type != cell_type_e::DOUBLE) {
         throw runtime_exception_c(
             "not command expects parameter to evaluate to a numerical type",
             cells[1].location);
      }

      if (std::stod(target.data) > 0.0) {
         return {CELL_FALSE};
      } else {
         return {CELL_TRUE};
      }
   });

   _builtins[BUILTIN_ASSERT] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() < 3) {
             throw runtime_exception_c(
                 "assert command expects at least 3 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          if (cells[1].type != cell_type_e::STRING) {
             throw runtime_exception_c("assertion label bust be a raw string",
                                       cells[1].location);
          }

          for (auto c = cells.begin() + 2; c != cells.end(); c++) {

             auto result = process_cell((*c), env);

             if (static_cast<int>(result.type) <
                 static_cast<int>(cell_type_e::STRING)) {
                throw runtime_exception_c(
                    "assertion condition did not evaluate to a direectly "
                    "comparable type (string, int, double)",
                    (*c).location);
             }

             if (cell_type_e::STRING == result.type && result.data.empty()) {
                throw assertion_exception_c(
                    "assertion failure: " + cells[1].data, (*c).location);
             } else if (cell_type_e::INTEGER == result.type &&
                        std::stoull(result.data) < 1) {
                throw assertion_exception_c(
                    "assertion failure: " + cells[1].data, (*c).location);
             } else if (cell_type_e::DOUBLE == result.type &&
                        std::stod(result.data) <= 0.0) {
                throw assertion_exception_c(
                    "assertion failure: " + cells[1].data, (*c).location);
             }
          }
          return {CELL_TRUE};
       });

   _builtins[BUILTIN_VAR] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() < 2) {
             throw runtime_exception_c("Nothing given to var command",
                                       cells[0].location);
          }

          auto &variable_name = cells[1].data;

          if (variable_name.find('.') != std::string::npos) {
             throw runtime_exception_c(
                 "Attempting to directly define a variable accessor " +
                     variable_name,
                 cells[1].location);
          }

          if (cells[1].builtin_encoding != BUILTIN_DEFAULT_VAL) {
             throw runtime_exception_c("Attempting to define a key symbol: " +
                                           variable_name,
                                       cells[1].location);
          }

          if (cells.size() == 2) {
             auto cell = cell_c(cell_type_e::LIST, "<list>");
             env->set(variable_name, cell);
             return {cell};
          }

          if (cells.size() != 3) {
             throw runtime_exception_c(
                 "var command expects 2 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto value = process_cell(cells[2], env);

          if (value.type == cell_type_e::SYMBOL) {
             throw runtime_exception_c("Expected list or datum value (var)",
                                       cells[2].location);
          }

          env->set(variable_name, value);
          return {value};
       });

   _builtins[BUILTIN_PUT] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          for (auto i = cells.begin() + 1; i != cells.end(); ++i) {

             auto item = process_cell((*i), env);

             std::string stringed;
             cell_to_string(stringed, item, env, false);
             std::cout << stringed;
          }

          return CELL_TRUE;
       });

   _builtins[BUILTIN_PUTLN] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          for (auto i = cells.begin() + 1; i != cells.end(); ++i) {

             auto item = process_cell((*i), env);

             std::string stringed;
             cell_to_string(stringed, item, env, false);
             std::cout << stringed;
          }
          std::cout << std::endl;

          return CELL_TRUE;
       });

   _builtins[BUILTIN_LAMBDA] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          // First item following lambda must be a list of parameters
          std::vector<cell_c> body(cells.begin() + 1, cells.end());
          cell_c lambda(body);
          lambda.type = cell_type_e::LAMBDA;
          lambda.data = cells[1].data;
          return {lambda};
       });

   _builtins[BUILTIN_LOOP] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 3 && cells.size() != 4) {
             throw runtime_exception_c(
                 "loop command expects 3 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto &conditional_cell = cells[1];

          cell_c *body{nullptr};
          cell_c *post{nullptr};

          if (cells.size() == 3) {
             body = &cells[2];
          } else if (cells.size() == 4) {
             post = &cells[2];
             body = &cells[3];
          }

          while (1) {

             auto loop_environment = std::make_shared<environment_c>(env);

             // Check the condition
             auto conditional_result = process_cell(conditional_cell, loop_environment);

             if (!eval_truthy(conditional_result, cells[0].location)) {
                break;
             }

             // Execute the body
             auto r = process_cell(*body, loop_environment);
             if (r.stop_processing) {
                break;
             }

             if (post) {
                process_cell(*post, loop_environment);
             }
          }

          return {CELL_NIL};
       });

   _builtins[BUILTIN_SET] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 3) {
             throw runtime_exception_c(
                 "set command expects 2 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto &variable_name = cells[1].data;

          if (variable_name.find('.') != std::string::npos) {

             auto accessors = retrieve_accessors(variable_name);

             cell_c result;
             std::shared_ptr<environment_c> target_env = env;
             for (std::size_t i = 0; i < accessors.size(); i++) {

                // Get the item from the accessor
                auto containing_env =
                    target_env->find(accessors[i], cells[1].location);
                result = containing_env->get(accessors[i]);

                // Check if we need to move the environment "in" to the next
                // object
                if (result.type == cell_type_e::BOX) {
                   target_env = result.box_env;
                }
             }

             auto value = process_cell(cells[2], env);

             if (value.type == cell_type_e::SYMBOL) {
                throw runtime_exception_c("Expected list or datum value (set)",
                                          cells[2].location);
             }

             target_env->set(accessors.back(), value);
             return {value};

          } else {
             // If this isn't found it will throw :)
             auto containing_env = env->find(variable_name, cells[1].location);
             auto value = process_cell(cells[2], env);

             if (value.type == cell_type_e::SYMBOL) {
                throw runtime_exception_c("Expected list or datum value (set)",
                                          cells[2].location);
             }

             containing_env->set(variable_name, value);
             return {value};
          }
       });

   // List and block are extremely similar, and realistically `list` coult be
   // used instead of `block` but its "less efficient" as it does the work to
   // construct what would be a temporary cell, while `block` does not.
   _builtins[BUILTIN_BLOCK] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          for (size_t i = 1; i < cells.size() - 1; i++) {
             auto r = process_cell(cells[i], env);
             if (r.stop_processing) {
                // We want to pass the stop flag up
                return r;
             }
          }

          return process_cell(cells[cells.size() - 1], env);
       });

   _builtins[BUILTIN_LIST] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          std::vector<cell_c> body;

          for (auto i = cells.begin() + 1; i != cells.end(); ++i) {
             body.push_back(process_cell(*i, env));
          }

          cell_c list(body);
          list.type = cell_type_e::LIST;
          list.data = "<list>";
          return {list};
       });

   _builtins[BUILTIN_COMPOSE] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "compose command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          std::string value;
          for (auto c = cells.begin() + 1; c < cells.end(); c++) {
             quote_cell(value, (*c), env);
          }

          return cell_c(cell_type_e::STRING, value);
       });

   _builtins[BUILTIN_DECOMPOSE] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "decompose command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto target = process_cell(cells[1], env);

          cell_c result;
          eval_c evaluator(
              env, [&result](cell_c cell) { result = cell; });

          evaluator.eval(cells[1].location.line, target.data);
          return result;
       });

   _builtins[BUILTIN_BOX] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 3) {
             throw runtime_exception_c(
                 "object command expectes 3 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto &variable_name = cells[1].data;

          if (cells[1].builtin_encoding != BUILTIN_DEFAULT_VAL) {
             throw runtime_exception_c("Attempting to define a key symbol: " +
                                           variable_name,
                                       cells[1].location);
          }

          auto object_cell = cell_c(cell_type_e::BOX);
          object_cell.box_env = std::make_shared<sauros::environment_c>(env);

          // Result of loading object body is
          process_cell(cells[2], object_cell.box_env);

          env->set(variable_name, object_cell);
          return {CELL_TRUE};
       });

   _builtins[BUILTIN_TRUE] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          return {sauros::CELL_TRUE};
       });

   _builtins[BUILTIN_FALSE] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          return {sauros::CELL_FALSE};
       });

   _builtins[BUILTIN_NIL] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          return {sauros::CELL_NIL};
       });

   _builtins[BUILTIN_IS_NIL] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "is_null expects only one parameter to evaluate",
                 cells[0].location);
          }

          return (process_cell(cells[1], env).data == sauros::CELL_NIL.data)
                     ? sauros::CELL_TRUE
                     : sauros::CELL_FALSE;
       });

   _builtins[BUILTIN_LEN] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "len command expects 2 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          return {cell_c(cell_type_e::INTEGER,
                         std::to_string(process_cell(cells[1], env).list.size()),
                         cells[1].location)};
       });

   _builtins[BUILTIN_IF] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 3 && cells.size() != 4) {
             throw runtime_exception_c(
                 "if command expects 2-3 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto eval = process_cell(cells[1], env);

          if (eval_truthy(eval, cells[0].location)) {
             return process_cell(cells[2], env);
          } else if (cells.size() == 4) {
             return process_cell(cells[3], env);
          }
          return CELL_TRUE;
       });

   _builtins[BUILTIN_SEQ] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 3) {
             throw runtime_exception_c(
                 "seq command expects 3 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto lhs = process_cell(cells[1], env);
          auto rhs = process_cell(cells[2], env);

          return cell_c(cell_type_e::INTEGER,
                        std::to_string((lhs.data == rhs.data)),
                        cells[0].location);
       });

   _builtins[BUILTIN_SNEQ] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> cell_c {
          if (cells.size() != 3) {
             throw runtime_exception_c(
                 "sneq command expects 3 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto lhs = process_cell(cells[1], env);
          auto rhs = process_cell(cells[2], env);

          return cell_c(cell_type_e::INTEGER,
                        std::to_string((lhs.data != rhs.data)),
                        cells[0].location);
       });

   _builtins[BUILTIN_LT] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          return {perform_arithmetic(
              "<", cells,
              [](double lhs, double rhs) -> double { return lhs < rhs; }, env)};
       });

   _builtins[BUILTIN_LT_EQ] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          return {perform_arithmetic(
              "<=", cells,
              [](double lhs, double rhs) -> double { return lhs <= rhs; },
              env)};
       });

   _builtins[BUILTIN_GT] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          return {perform_arithmetic(
              ">", cells,
              [](double lhs, double rhs) -> double { return lhs > rhs; }, env)};
       });

   _builtins[BUILTIN_GT_EQ] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          return {perform_arithmetic(
              ">=", cells,
              [](double lhs, double rhs) -> double { return lhs >= rhs; },
              env)};
       });

   _builtins[BUILTIN_EQ_EQ] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          return {perform_arithmetic(
              "==", cells,
              [](double lhs, double rhs) -> double { return lhs == rhs; },
              env)};
       });

   _builtins[BUILTIN_NOT_EQ] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          return {perform_arithmetic(
              "!=", cells,
              [](double lhs, double rhs) -> double { return lhs != rhs; },
              env)};
       });

   _builtins[BUILTIN_ADD] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          return {perform_arithmetic(
              "+", cells,
              [](double lhs, double rhs) -> double { return lhs + rhs; }, env)};
       });

   _builtins[BUILTIN_SUB] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          return {perform_arithmetic(
              "-", cells,
              [](double lhs, double rhs) -> double { return lhs - rhs; }, env)};
       });

   _builtins[BUILTIN_DIV] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          return {perform_arithmetic(
              "/", cells,
              [=](double lhs, double rhs) -> double {
                 if (rhs == 0.0) {
                    return 0.0;
                 }

                 return lhs / rhs;
              },
              env, true)};
       });

   _builtins[BUILTIN_MUL] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          return {perform_arithmetic(
              "*", cells,
              [](double lhs, double rhs) -> double { return lhs * rhs; }, env)};
       });

   _builtins[BUILTIN_MOD] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          return {perform_arithmetic(
              "%", cells,
              [](double lhs, double rhs) -> double {
                 return static_cast<int64_t>(lhs) % static_cast<int64_t>(rhs);
              },
              env)};
       });

   _builtins[BUILTIN_OR] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          return {perform_arithmetic(
              "%", cells,
              [](double lhs, double rhs) -> double {
                 bool result =
                     (static_cast<bool>(lhs) || static_cast<bool>(rhs)) ? true
                                                                        : false;
                 return result;
              },
              env)};
       });

   _builtins[BUILTIN_AND] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          return {perform_arithmetic(
              "%", cells,
              [](double lhs, double rhs) -> double {
                 bool result =
                     (static_cast<bool>(lhs) && static_cast<bool>(rhs)) ? true
                                                                        : false;
                 return result;
              },
              env)};
       });

   _builtins[BUILTIN_XOR] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> cell_c {
          return {perform_arithmetic(
              "%", cells,
              [](double lhs, double rhs) -> double {
                 auto _lhs = static_cast<bool>(lhs);
                 auto _rhs = static_cast<bool>(rhs);
                 bool result =
                     ((_lhs && !_rhs) || (_rhs && !_lhs)) ? true : false;
                 return result;
              },
              env)};
       });
}

} // namespace sauros