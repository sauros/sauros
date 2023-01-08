#include "processor.hpp"
#include "sauros/driver.hpp"
#include "sauros/profiler.hpp"
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace sauros {

namespace {

static inline bool eval_truthy(cell_ptr cell, location_s *location) {
#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("processor_builtins.cpp :: eval_truthy");
#endif

   switch (cell->type) {
   case cell_type_e::STRING:
      return (!cell->data.empty());
      break;
   case cell_type_e::LIST:
      return (!cell->list.empty());
      break;
   case cell_type_e::LAMBDA:
      return true;
      break;
   case cell_type_e::REAL:
      [[fallthrough]];
   case cell_type_e::INTEGER: {
      try {
         auto v = std::stod(cell->data);
         return (v > 0.0);
      } catch (const std::invalid_argument &) {
         throw processor_c::runtime_exception_c(
             "Invalid data type given for operand",
             std::make_shared<cell_c>(cell_type_e::STRING, "", location));
      } catch (const std::out_of_range &) {
         throw processor_c::runtime_exception_c(
             "Item caused out of range exception",
             std::make_shared<cell_c>(cell_type_e::STRING, "", location));
      }
      break;
   }
   }
   return false;
}

} // namespace

void processor_c::populate_standard_builtins() {

   auto expect_var_get_name = [this](cell_ptr cell) -> std::string {
      if (cell->data.find('.') != std::string::npos) {
         throw runtime_exception_c(
             "Attempting to directly define a variable accessor " + cell->data,
             cell);
      }

      if (cell->builtin_encoding != BUILTIN_DEFAULT_VAL) {
         throw runtime_exception_c(
             "Attempting to define a key symbol: " + cell->data, cell);
      }
      return cell->data;
   };

   _builtins[BUILTIN_IMPORT] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::IMPORT");
#endif
          if (cells.size() < 2) {
             throw runtime_exception_c(
                 "import command expects at least 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          file_executor_c loader(env);
          for (auto i = cells.begin() + 1; i < cells.end(); i++) {
             if ((*i)->type != sauros::cell_type_e::STRING) {
                throw sauros::processor_c::runtime_exception_c(
                    "Import objects are expected to be raw strings", (*i));
             }
             if (0 != loader.run((*i)->data)) {
                bool success{false};
                auto sys_dir = _system.get_sauros_directory();
                if (sys_dir.has_value()) {
                   std::filesystem::path p = (*sys_dir);
                   p /= (*i)->data;
                   success = (0 == loader.run(p));
                }

                if (!success) {
                   throw sauros::processor_c::runtime_exception_c(
                       "Unable to load import: " + (*i)->data, (*i));
                }
             }
          }
          return std::make_shared<cell_c>(CELL_TRUE);
       });

   _builtins[BUILTIN_USE] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::USE");
#endif
          if (cells.size() < 2) {
             throw runtime_exception_c(
                 "use command expects at least 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          for (auto i = cells.begin() + 1; i < cells.end(); i++) {
             if ((*i)->type != sauros::cell_type_e::STRING) {
                throw sauros::processor_c::runtime_exception_c(
                    "use command expects parameters to be raw strings", (*i));
             }

             load_package((*i)->data, (*i)->location, env);
          }

          return std::make_shared<cell_c>(CELL_TRUE);
       });

   _builtins[BUILTIN_EXIT] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::EXIT");
#endif
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "exit command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          std::exit(std::stoull(process_cell(cells[1], env)->data));
       });

   _builtins[BUILTIN_BREAK] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::BREAK");
#endif
          if (cells.size() != 1) {
             throw runtime_exception_c(
                 "break command expects 0 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }
          _break_loop = true;
          return std::make_shared<cell_c>(CELL_NIL);
       });

   _builtins[BUILTIN_TYPE] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::TYPE");
#endif
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "type command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          auto target = process_cell(cells[1], env);

          return {std::make_shared<cell_c>(cell_type_e::STRING,
                                           cell_type_to_string(target->type),
                                           cells[1]->location)};
       });

   _builtins[BUILTIN_FRONT] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::FRONT");
#endif
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "front command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          auto target = process_cell(cells[1], env);

          if (!(target->type == cell_type_e::LIST)) {
             throw runtime_exception_c("Expected list parameter for front",
                                       cells[1]);
          }

          if (target->list.empty()) {
             return std::make_shared<cell_c>(CELL_NIL);
          }

          return {target->list[0]};
       });

   _builtins[BUILTIN_BACK] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::BACK");
#endif
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "back command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          auto target = process_cell(cells[1], env);

          if (!(target->type == cell_type_e::LIST)) {
             throw runtime_exception_c("Expected list parameter for back",
                                       cells[1]);
          }

          if (target->list.empty()) {
             return std::make_shared<cell_c>(CELL_NIL);
          }

          return {*(target->list.end() - 1)};
       });

   _builtins[BUILTIN_AT] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::AT");
#endif
          if (cells.size() != 3) {
             throw runtime_exception_c("at command expects 2 parameters, but " +
                                           std::to_string(cells.size() - 1) +
                                           " were given",
                                       cells[0]);
          }

          auto index = process_cell(cells[1], env);
          if (index->type != cell_type_e::INTEGER) {
             throw runtime_exception_c(
                 "at command index must me an integer type", cells[0]);
          }

          uint64_t idx = std::stoull(index->data);

          if (cells[2]->type != cell_type_e::SYMBOL) {
             throw runtime_exception_c(
                 "Second parameter of at must be a variable", cells[2]);
          }

          auto target = load_potential_variable(cells[2], env);

          if (target->list.size() <= idx) {
             return std::make_shared<cell_c>(CELL_NIL);
          }
          return {target->list[idx]};
       });

   _builtins[BUILTIN_CLEAR] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::CLEAR");
#endif
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "clear command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          if (cells[1]->type != cell_type_e::SYMBOL) {
             throw runtime_exception_c(
                 "First parameter of clear must be a variable", cells[1]);
          }

          auto variable = process_cell(cells[1], env);
          variable->list.clear();
          return std::make_shared<cell_c>(CELL_TRUE);
       });

   _builtins[BUILTIN_POP] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::POP");
#endif
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "pop command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          if (cells[1]->type != cell_type_e::SYMBOL) {
             throw runtime_exception_c(
                 "First parameter of pop must be a variable", cells[1]);
          }

          auto variable = process_cell(cells[1], env);
          if (!variable->list.empty()) {
             variable->list.pop_back();
          }
          return std::make_shared<cell_c>(CELL_TRUE);
       });

   _builtins[BUILTIN_PUSH] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::PUSH");
#endif
          if (cells.size() != 3) {
             throw runtime_exception_c(
                 "push command expects 2 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          if (cells[1]->type != cell_type_e::SYMBOL) {
             throw runtime_exception_c(
                 "First parameter of push must be a variable", cells[1]);
          }

          auto variable = process_cell(cells[1], env);
          auto value = process_cell(cells[2], env);

          variable->list.push_back(value->clone());
          return std::make_shared<cell_c>(CELL_TRUE);
       });

   _builtins[BUILTIN_THROW] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::THROW");
#endif
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "throw command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          auto message = process_cell(cells[1], env);

          if (message->type != cell_type_e::STRING) {
             throw runtime_exception_c(
                 "throw command expects second parameter to be a string type",
                 cells[0]);
          }

          throw runtime_exception_c(message->data, cells[0]);
       });

   _builtins[BUILTIN_NOT] = std::make_shared<
       cell_c>([this](cells_t &cells,
                      std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
      profiler_c::get_profiler()->hit("processor_builtin::NOT");
#endif
      if (cells.size() != 2) {
         throw runtime_exception_c("not command expects 1 parameters, but " +
                                       std::to_string(cells.size() - 1) +
                                       " were given",
                                   cells[0]);
      }

      auto target = process_cell(cells[1], env);

      if (target->type != cell_type_e::INTEGER &&
          target->type != cell_type_e::REAL) {
         throw runtime_exception_c(
             "not command expects parameter to evaluate to a numerical type",
             cells[1]);
      }

      if (std::stod(target->data) > 0.0) {
         return std::make_shared<cell_c>(CELL_FALSE);
      } else {
         return std::make_shared<cell_c>(CELL_TRUE);
      }
   });

   _builtins[BUILTIN_ASSERT] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::ASSERT");
#endif
          if (cells.size() < 3) {
             throw runtime_exception_c(
                 "assert command expects at least 3 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          if (cells[1]->type != cell_type_e::STRING) {
             throw runtime_exception_c("assertion label bust be a raw string",
                                       cells[1]);
          }

          for (auto c = cells.begin() + 2; c != cells.end(); c++) {

             auto result = process_cell((*c), env);

             if (static_cast<unsigned short>(result->type) >
                 static_cast<unsigned short>(cell_type_e::STRING)) {
                throw runtime_exception_c(
                    "assertion condition did not evaluate to a direectly "
                    "comparable type (string, int, double)",
                    (*c));
             }

             if (cell_type_e::STRING == result->type && result->data.empty()) {
                throw assertion_exception_c(
                    "assertion failure: " + cells[1]->data, cells[0]);
             } else if (cell_type_e::INTEGER == result->type &&
                        std::stoull(result->data) < 1) {
                throw assertion_exception_c(
                    "assertion failure: " + cells[1]->data, cells[0]);
             } else if (cell_type_e::REAL == result->type &&
                        std::stod(result->data) <= 0.0) {
                throw assertion_exception_c(
                    "assertion failure: " + cells[1]->data, cells[0]);
             }
          }
          return std::make_shared<cell_c>(CELL_TRUE);
       });

   _builtins[BUILTIN_VAR] = std::make_shared<cell_c>(
       [this, expect_var_get_name](
           cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::VAR");
#endif

          if (cells.size() < 2) {
             throw runtime_exception_c("Nothing given to var command",
                                       cells[0]);
          }

          auto variable_name = expect_var_get_name(cells[1]);

          if (cells.size() == 2) {
             auto cell = std::make_shared<cell_c>(cell_type_e::LIST, "<list>");
             env->set(variable_name, cell);
             return {cell};
          }

          if (cells.size() != 3) {
             throw runtime_exception_c(
                 "var command expects 2 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          auto value = process_cell(cells[2], env);

          if (value->type == cell_type_e::SYMBOL) {
             throw runtime_exception_c("Expected list or datum value (var)",
                                       cells[2]);
          }

          env->set(variable_name, value);
          return {value};
       });

   _builtins[BUILTIN_PUT] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::PUT");
#endif
          for (auto i = cells.begin() + 1; i != cells.end(); ++i) {

             auto item = process_cell((*i), env);

             std::string stringed;
             cell_to_string(stringed, item, env, false);
             std::cout << stringed << std::flush;
          }

          return std::make_shared<cell_c>(CELL_TRUE);
       });

   _builtins[BUILTIN_YIELD] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::YIELD");
#endif
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "Yield command expects only one parameter", cells[0]);
          }
          _yield_cell = process_cell(cells[1], env);
          return _yield_cell;
       });

   _builtins[BUILTIN_PUTLN] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::PUTLN");
#endif
          for (auto i = cells.begin() + 1; i != cells.end(); ++i) {

             auto item = process_cell((*i), env);

             std::string stringed;
             cell_to_string(stringed, item, env, false);
             std::cout << stringed;
          }
          std::cout << std::endl << std::flush;

          return std::make_shared<cell_c>(CELL_TRUE);
       });

   _builtins[BUILTIN_LAMBDA] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::LAMBDA");
#endif
          // First item following lambda must be a list of parameters
          cells_t body(cells.begin() + 1, cells.end());
          cell_ptr lambda = std::make_shared<cell_c>(body);
          lambda->type = cell_type_e::LAMBDA;
          lambda->data = cells[1]->data;
          return {lambda};
       });

   _builtins[BUILTIN_ITER] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::ITER");
#endif
          if (cells.size() != 4) {
             throw runtime_exception_c(
                 "iter command expects 3 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          if (cells[1]->type != cell_type_e::SYMBOL) {
             throw runtime_exception_c(
                 "first parameter to iter needs to be a symbol", cells[1]);
          }
          auto iter_var = cells[1]->data;

          auto processed_list = load_potential_variable(cells[2], env);
          if (processed_list->type != cell_type_e::LIST) {
             throw runtime_exception_c(
                 "second parameter to iter needs to be a list to iterate over",
                 cells[2]);
          }

          auto body = &cells[3];
          for (auto &c : processed_list->list) {

             // This will store the iter value for the user to access
             auto temp_iter_env = std::make_shared<environment_c>(env);

             // Put the cell in the env as the iter_val
             temp_iter_env->set(iter_var, c);

             // Execute the body
             auto r = process_cell(*body, temp_iter_env);
             c = temp_iter_env->get(iter_var)->clone();
             if (_yield_cell) {
                return _yield_cell;
             }
             if (_break_loop) {
                _break_loop = false;
                break;
             }
          }
          return std::make_shared<cell_c>(CELL_NIL);
       });

   _builtins[BUILTIN_LOOP] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::LOOP");
#endif
          if (cells.size() != 3 && cells.size() != 4) {
             throw runtime_exception_c(
                 "loop command expects 3 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          auto &conditional_cell = cells[1];

          cell_ptr *body{nullptr};
          cell_ptr *post{nullptr};

          if (cells.size() == 3) {
             body = &cells[2];
          } else if (cells.size() == 4) {
             post = &cells[2];
             body = &cells[3];
          }

          while (1) {

             auto loop_environment = std::make_shared<environment_c>(env);

             // Check the condition
             auto conditional_result =
                 process_cell(conditional_cell, loop_environment);

             if (!eval_truthy(conditional_result, cells[0]->location)) {
                break;
             }

             // Execute the body
             auto r = process_cell(*body, loop_environment);
             if (_break_loop) {
                _break_loop = false;
                break;
             }

             if (_yield_cell) {
                return _yield_cell;
             }

             if (post) {
                process_cell(*post, loop_environment);
             }
          }

          return std::make_shared<cell_c>(CELL_NIL);
       });

   _builtins[BUILTIN_SET] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::SET");
#endif
          if (cells.size() != 3) {
             throw runtime_exception_c(
                 "set command expects 2 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          auto &variable_name = cells[1]->data;

          if (variable_name.find('.') != std::string::npos) {

             auto [_, target_variable, target_env] =
                 retrieve_box_data(cells[1], env);

             auto value = process_cell(cells[2], env);

             if (value->type == cell_type_e::SYMBOL) {
                throw runtime_exception_c("Expected list or datum value (set)",
                                          cells[2]);
             }

             target_env->set(target_variable, value);
             return {value};

          } else {
             // If this isn't found it will throw :)
             auto containing_env = env->find(variable_name, cells[1]);
             auto value = process_cell(cells[2], env);

             if (value->type == cell_type_e::SYMBOL) {
                throw runtime_exception_c("Expected list or datum value (set)",
                                          cells[2]);
             }

             containing_env->set(variable_name, value);
             return {value};
          }
       });

   _builtins[BUILTIN_SET_AT] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::SET_AT");
#endif
          if (cells.size() != 4) {
             throw runtime_exception_c(
                 "set_at command expects 3 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          auto idx = process_cell(cells[1], env);
          if (idx->type != cell_type_e::INTEGER) {
             throw runtime_exception_c("set_at requires index to be an integer",
                                       cells[1]);
          }

          auto target_value = process_cell(cells[3], env);
          auto target_var = load_potential_variable(cells[2], env);
          target_var->list[std::stoull(idx->data)] = target_value;
          return std::make_shared<cell_c>(CELL_TRUE);
       });

   // List and block are extremely similar, and realistically `list` coult be
   // used instead of `block` but its "less efficient" as it does the work to
   // construct what would be a temporary cell, while `block` does not.
   _builtins[BUILTIN_BLOCK] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::BLOCK");
#endif
          for (size_t i = 1; i < cells.size() - 1; i++) {
             auto r = process_cell(cells[i], env);
             if (_yield_cell) {
                return _yield_cell;
             }
             if (_break_loop) {
                // Don't reset _break_loop so it is passed up
                return std::make_shared<cell_c>(CELL_NIL);
             }
          }

          if (_yield_cell) {
             return _yield_cell;
          }

          return process_cell(cells[cells.size() - 1], env);
       });

   _builtins[BUILTIN_LIST] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::LIST");
#endif
          cells_t body;

          for (auto i = cells.begin() + 1; i != cells.end(); ++i) {
             body.push_back(process_cell(*i, env));
          }

          cell_ptr list = std::make_shared<cell_c>(body);
          list->type = cell_type_e::LIST;
          list->data = "<list>";
          return {list};
       });

   _builtins[BUILTIN_TRY] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::TRY");
#endif
          if (cells.size() != 3) {
             throw runtime_exception_c(
                 "try command expects 2 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          auto make_error = [this, cells,
                             env](const char *message) -> cell_ptr {
             auto temp_env = std::make_shared<environment_c>(env);
             temp_env->set(
                 "$", std::make_shared<cell_c>(cell_type_e::STRING, message));
             return process_cell(cells[2], temp_env);
          };

          try {
             return process_cell(cells[1], env);
          } catch (sauros::processor_c::runtime_exception_c &e) {
             return make_error(e.what());
          } catch (sauros::processor_c::assertion_exception_c &e) {
             return make_error(e.what());
          } catch (sauros::environment_c::unknown_identifier_c &e) {
             return make_error(e.what());
          } catch (sauros::parser::parser_exception_c &e) {
             return make_error(e.what());
          }
       });

   _builtins[BUILTIN_COMPOSE] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::COMPOSE");
#endif
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "compose command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          std::string value;
          for (auto c = cells.begin() + 1; c < cells.end(); c++) {
             quote_cell(value, (*c), env);
          }

          return std::make_shared<cell_c>(cell_type_e::STRING, value);
       });

   _builtins[BUILTIN_DECOMPOSE] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::DECOMPOSE");
#endif
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "decompose command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          auto target = process_cell(cells[1], env);

          cell_ptr result;
          eval_c evaluator(env, [&result](cell_ptr cell) { result = cell; });

          evaluator.eval(cells[1]->location->line, target->data);
          return result;
       });

   _builtins[BUILTIN_BOX] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::BOX");
#endif
          if (cells.size() != 3) {
             throw runtime_exception_c(
                 "object command expectes 3 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          auto &variable_name = cells[1]->data;

          if (cells[1]->builtin_encoding != BUILTIN_DEFAULT_VAL) {
             throw runtime_exception_c("Attempting to define a key symbol: " +
                                           variable_name,
                                       cells[1]);
          }

          auto object_cell = std::make_shared<cell_c>(cell_type_e::BOX);
          object_cell->box_env = std::make_shared<sauros::environment_c>(env);

          // Result of loading object body is
          process_cell(cells[2], object_cell->box_env);

          env->set(variable_name, object_cell);
          return std::make_shared<cell_c>(CELL_TRUE);
       });

   _builtins[BUILTIN_TRUE] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::TRUE");
#endif
          return std::make_shared<cell_c>(CELL_TRUE);
       });

   _builtins[BUILTIN_FALSE] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::FALSE");
#endif
          return std::make_shared<cell_c>(CELL_FALSE);
       });

   _builtins[BUILTIN_NIL] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::NIL");
#endif
          return std::make_shared<cell_c>(CELL_NIL);
       });

   _builtins[BUILTIN_IS_NIL] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::IS_NIL");
#endif
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "is_null expects only one parameter to evaluate", cells[0]);
          }

          return (process_cell(cells[1], env)->data == CELL_NIL.data)
                     ? std::make_shared<cell_c>(sauros::CELL_TRUE)
                     : std::make_shared<cell_c>(sauros::CELL_FALSE);
       });

   _builtins[BUILTIN_LEN] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::LEN");
#endif
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "len command expects 2 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          return {std::make_shared<cell_c>(
              cell_type_e::INTEGER,
              std::to_string(process_cell(cells[1], env)->list.size()),
              cells[1]->location)};
       });

   _builtins[BUILTIN_REV] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::REV");
#endif
          if (cells.size() != 2) {
             throw runtime_exception_c("rev command expects 1 parameter, but " +
                                           std::to_string(cells.size() - 1) +
                                           " were given",
                                       cells[0]);
          }

          auto target = process_cell(cells[1], env);
          if (target->type != cell_type_e::LIST) {
             throw runtime_exception_c("rev expects parameter to be a list",
                                       cells[1]);
          }

          auto result = std::make_shared<cell_c>(cell_type_e::LIST);
          result->list = target->list;
          std::reverse(result->list.begin(), result->list.end());
          return result;
       });

   _builtins[BUILTIN_IF] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::IF");
#endif
          if (cells.size() != 3 && cells.size() != 4) {
             throw runtime_exception_c(
                 "if command expects 2-3 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          auto eval = process_cell(cells[1], env);

          if (eval_truthy(eval, cells[0]->location)) {
             return process_cell(cells[2], env);
          } else if (cells.size() == 4) {
             return process_cell(cells[3], env);
          }
          return std::make_shared<cell_c>(CELL_TRUE);
       });

   _builtins[BUILTIN_SEQ] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::SEQ");
#endif
          if (cells.size() != 3) {
             throw runtime_exception_c(
                 "seq command expects 3 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          auto lhs = process_cell(cells[1], env);
          auto rhs = process_cell(cells[2], env);

          return std::make_shared<cell_c>(
              cell_type_e::INTEGER, std::to_string((lhs->data == rhs->data)),
              cells[0]->location);
       });

   _builtins[BUILTIN_SNEQ] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::SNEQ");
#endif
          if (cells.size() != 3) {
             throw runtime_exception_c(
                 "sneq command expects 3 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          auto lhs = process_cell(cells[1], env);
          auto rhs = process_cell(cells[2], env);

          return std::make_shared<cell_c>(
              cell_type_e::INTEGER, std::to_string((lhs->data != rhs->data)),
              cells[0]->location);
       });

   _builtins[BUILTIN_LT] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::LT");
#endif
          return {perform_arithmetic(
              "<", cells,
              [](double lhs, double rhs) -> double { return lhs < rhs; }, env)};
       });

   _builtins[BUILTIN_LT_EQ] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::LT_EQ");
#endif
          return {perform_arithmetic(
              "<=", cells,
              [](double lhs, double rhs) -> double { return lhs <= rhs; },
              env)};
       });

   _builtins[BUILTIN_GT] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::GT");
#endif
          return {perform_arithmetic(
              ">", cells,
              [](double lhs, double rhs) -> double { return lhs > rhs; }, env)};
       });

   _builtins[BUILTIN_GT_EQ] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::GT_EQ");
#endif
          return {perform_arithmetic(
              ">=", cells,
              [](double lhs, double rhs) -> double { return lhs >= rhs; },
              env)};
       });

   _builtins[BUILTIN_EQ_EQ] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::EQ_EQ");
#endif
          return {perform_arithmetic(
              "==", cells,
              [](double lhs, double rhs) -> double { return lhs == rhs; },
              env)};
       });

   _builtins[BUILTIN_NOT_EQ] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::NOT_EQ");
#endif
          return {perform_arithmetic(
              "!=", cells,
              [](double lhs, double rhs) -> double { return lhs != rhs; },
              env)};
       });

   _builtins[BUILTIN_ADD] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::ADD");
#endif
          return {perform_arithmetic(
              "+", cells,
              [](double lhs, double rhs) -> double { return lhs + rhs; }, env)};
       });

   _builtins[BUILTIN_SUB] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::SUB");
#endif
          return {perform_arithmetic(
              "-", cells,
              [](double lhs, double rhs) -> double { return lhs - rhs; }, env)};
       });

   _builtins[BUILTIN_DIV] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::DIV");
#endif
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

   _builtins[BUILTIN_MUL] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::MUL");
#endif
          return {perform_arithmetic(
              "*", cells,
              [](double lhs, double rhs) -> double { return lhs * rhs; }, env)};
       });

   _builtins[BUILTIN_MOD] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::MOD");
#endif
          return {perform_arithmetic(
              "%", cells,
              [](double lhs, double rhs) -> double {
                 return static_cast<int64_t>(lhs) % static_cast<int64_t>(rhs);
              },
              env)};
       });

   _builtins[BUILTIN_OR] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::OR");
#endif
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

   _builtins[BUILTIN_AND] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::AND");
#endif
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

   _builtins[BUILTIN_XOR] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::XOR");
#endif
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

   _builtins[BUILTIN_BITWISE_AND] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::BW_AND");
#endif
          return {perform_arithmetic(
              "%", cells,
              [](double lhs, double rhs) -> double {
                 return static_cast<uint64_t>(lhs) & static_cast<uint64_t>(rhs);
              },
              env)};
       });

   _builtins[BUILTIN_BITWISE_OR] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::BW_OR");
#endif
          return {perform_arithmetic(
              "%", cells,
              [](double lhs, double rhs) -> double {
                 return static_cast<uint64_t>(lhs) | static_cast<uint64_t>(rhs);
              },
              env)};
       });

   _builtins[BUILTIN_BITWISE_LSH] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::BW_LSH");
#endif
          return {perform_arithmetic(
              "%", cells,
              [](double lhs, double rhs) -> double {
                 return static_cast<uint64_t>(lhs)
                        << static_cast<uint64_t>(rhs);
              },
              env)};
       });

   _builtins[BUILTIN_BITWISE_RSH] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::BW_RSH");
#endif
          return {perform_arithmetic(
              "%", cells,
              [](double lhs, double rhs) -> double {
                 return static_cast<uint64_t>(lhs) >>
                        static_cast<uint64_t>(rhs);
              },
              env)};
       });

   _builtins[BUILTIN_BITWISE_XOR] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::BW_XOR");
#endif
          return {perform_arithmetic(
              "%", cells,
              [](double lhs, double rhs) -> double {
                 return static_cast<uint64_t>(lhs) ^ static_cast<uint64_t>(rhs);
              },
              env)};
       });

   _builtins[BUILTIN_BITWISE_NOT] = std::make_shared<
       cell_c>([this](cells_t &cells,
                      std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
      profiler_c::get_profiler()->hit("processor_builtin::BW_NOT");
#endif
      if (cells.size() != 2) {
         throw runtime_exception_c("bw_not command expects 1 parameters, but " +
                                       std::to_string(cells.size() - 1) +
                                       " were given",
                                   cells[0]);
      }

      auto target = process_cell(cells[1], env);

      if (target->type != cell_type_e::INTEGER &&
          target->type != cell_type_e::REAL) {
         throw runtime_exception_c(
             "not command expects parameter to evaluate to a numerical type",
             cells[0]);
      }

      auto val = static_cast<int64_t>(std::stod(target->data));
      return std::make_shared<cell_c>(cell_type_e::INTEGER,
                                      std::to_string(~val));
   });

   auto conversion_fn =
       [this](cell_ptr target, std::shared_ptr<environment_c> env,
              std::function<cell_ptr(cell_ptr source)> fn) -> cell_ptr {
      auto item = process_cell(target, env);
      try {
         return fn(item);
      } catch (const std::invalid_argument &) {
         throw runtime_exception_c("Invalid data type given for conversion",
                                   target);
      } catch (const std::out_of_range &) {
         throw runtime_exception_c("Item caused out of range exception",
                                   target);
      }
   };

   _builtins[BUILTIN_AS_INT] = std::make_shared<cell_c>(
       [this, conversion_fn](cells_t &cells,
                             std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::AS_INT");
#endif

          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "as_int command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }
          return conversion_fn(cells[1], env, [](cell_ptr target) -> cell_ptr {
             return std::make_shared<cell_c>(
                 cell_type_e::INTEGER,
                 std::to_string(std::stoull(target->data)));
          });
       });

   _builtins[BUILTIN_AS_STR] = std::make_shared<cell_c>(
       [this, conversion_fn](cells_t &cells,
                             std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::AS_STR");
#endif
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "as_str command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          // treat lists differently so it has behavior different than `compose`
          auto target = process_cell(cells[1], env);
          if (target->type == cell_type_e::LIST) {
             std::string result;
             for (auto e : target->list) {
                auto r = process_cell(e, env);
                result += r->data;
             }
             return std::make_shared<cell_c>(cell_type_e::STRING, result);
          }

          return conversion_fn(
              cells[1], env, [this, env](cell_ptr target) -> cell_ptr {
                 std::string result;
                 cell_to_string(result, target, env);
                 return std::make_shared<cell_c>(cell_type_e::STRING, result);
              });
       });

   _builtins[BUILTIN_AS_REAL] = std::make_shared<cell_c>(
       [this, conversion_fn](cells_t &cells,
                             std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::AS_REAL");
#endif
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "as_real command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }
          return conversion_fn(cells[1], env, [](cell_ptr target) -> cell_ptr {
             return std::make_shared<cell_c>(
                 cell_type_e::REAL, std::to_string(std::stod(target->data)));
          });
       });

   _builtins[BUILTIN_AS_LIST] = std::make_shared<cell_c>(
       [this, conversion_fn](cells_t &cells,
                             std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::AS_LIST");
#endif
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "as_list command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          auto target = process_cell(cells[1], env);
          if (static_cast<unsigned>(target->type) >
              static_cast<unsigned>(cell_type_e::STRING)) {
             throw runtime_exception_c("as_list command expects parameter to "
                                       "be integer, real, or string type",
                                       cells[1]);
          }

          cell_ptr result = std::make_shared<cell_c>(cell_type_e::LIST);
          for (auto c : target->data) {
             std::string as_individual = std::string(1, c);
             result->list.push_back(
                 std::make_shared<cell_c>(cell_type_e::STRING, as_individual));
          }
          return result;
       });

   _builtins[BUILTIN_ASYNC] = std::make_shared<cell_c>(
       [this, expect_var_get_name](
           cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::ASYNC");
#endif
          if (cells.size() != 3) {
             throw runtime_exception_c(
                 "async command expects 2 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          auto variable_name = expect_var_get_name(cells[1]);

          if (cells[2]->type != cell_type_e::LIST) {
             throw runtime_exception_c(
                 "async parameter required to be of type `list`\n"
                 "- A list that calls a symbol to execute asynchronously",
                 cells[2]);
          }

          // Setup async cell
          auto async_cell = std::make_shared<async_cell_c>(cells[0]->location);
          async_cell->processor = std::make_shared<processor_c>();

          auto box =
              std::make_shared<cell_c>(cell_type_e::BOX, cells[0]->location);

          box->box_env = std::make_shared<sauros::environment_c>();

          // Point the box.wait variable at the wait function of the async cell
          box->box_env->set("wait", async_cell->wait_fn);

          // Point the box.wait variable at the wait function of the async cell
          box->box_env->set("get", async_cell->get_fn);

          // Kick off the async
          async_cell->future =
              std::async(std::launch::async, &processor_c::process_cell,
                         *async_cell->processor, cells[2], env);

          // Store the async cell in the list to keep it alive
          box->list.push_back(async_cell);

          // Keep the target alive (might not be required yet)
          box->list.push_back(cells[1]);

          env->set(variable_name, box);
          return std::make_shared<cell_c>(CELL_TRUE);
       });

   _builtins[BUILTIN_CHAN] = std::make_shared<cell_c>(
       [this, expect_var_get_name](
           cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#ifdef PROFILER_ENABLED
          profiler_c::get_profiler()->hit("processor_builtin::ASYNC");
#endif
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "chan command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]);
          }

          auto variable_name = expect_var_get_name(cells[1]);

          auto chan_cell = std::make_shared<chan_cell_c>(cells[0]->location);
          chan_cell->processor = std::make_shared<processor_c>();

          auto box =
              std::make_shared<cell_c>(cell_type_e::BOX, cells[0]->location);
          box->box_env = std::make_shared<sauros::environment_c>();
          box->box_env->set("put", chan_cell->put_fn);
          box->box_env->set("get", chan_cell->get_fn);
          box->box_env->set("has_data", chan_cell->has_data_fn);
          box->box_env->set("drain", chan_cell->drain_fn);
          box->list.push_back(chan_cell);
          env->set(variable_name, box);
          return std::make_shared<cell_c>(CELL_TRUE);
       });
}

} // namespace sauros