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

   // Populate symbols that users are not allowed to redefine
   // - They wouldn't BE redefined as they exist in _builtins and not
   //   within the environment, but since they take precedence over
   //   the environment it will cause confusion if they call a redefinition
   //   and ours is executed.
   //
   //       May remove this in the future
   //
   //
   _key_symbols.insert("front");
   _key_symbols.insert("back");
   _key_symbols.insert("push");
   _key_symbols.insert("pop");
   _key_symbols.insert("list");
   _key_symbols.insert("set");
   _key_symbols.insert("lambda");
   _key_symbols.insert("block");
   _key_symbols.insert("len");
   _key_symbols.insert("put");
   _key_symbols.insert("putln");
   _key_symbols.insert("if");
   _key_symbols.insert("==");
   _key_symbols.insert("!=");
   _key_symbols.insert("<=");
   _key_symbols.insert(">=");
   _key_symbols.insert("<");
   _key_symbols.insert(">");
   _key_symbols.insert("seq");
   _key_symbols.insert("sneq");
   _key_symbols.insert("assert");
   _key_symbols.insert("loop");
   _key_symbols.insert("type");
   _key_symbols.insert("import");
   _key_symbols.insert("use");
   _key_symbols.insert("extern");
   _key_symbols.insert("not");
   _key_symbols.insert("or");
   _key_symbols.insert("and");
   _key_symbols.insert("xor");
   _key_symbols.insert("break");
   _key_symbols.insert("at");
   _key_symbols.insert("clear");
   _key_symbols.insert("compose");
   _key_symbols.insert("decompose");
   _key_symbols.insert("object");

   auto load = [&](cell_c &cell, std::shared_ptr<environment_c> env) -> cell_c {
      // std::cout << "TYPE: " << cell_type_to_string(cell.type) << " CELL: " <<
      // cell.data << std::endl;
      auto target = process_cell(cell, env);
      if (!target.has_value()) {
         throw runtime_exception_c("Unable to process value", cell.location);
      }
      return (*target);
   };

   _builtins["import"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
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
          return {sauros::CELL_TRUE};
       });

   _builtins["extern"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
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

             load_library((*i).data, (*i).location, env);
          }
          return {sauros::CELL_TRUE};
       });

   _builtins["use"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
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

   _builtins["exit"] =
       cell_c([this, load](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "exit command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          std::exit(std::stoull(load(cells[1], env).data));
       });

   _builtins["break"] =
       cell_c([this, load](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
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

   _builtins["type"] =
       cell_c([this, load](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "type command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto target = load(cells[1], env);

          return {cell_c(cell_type_e::STRING, cell_type_to_string(target.type),
                         cells[1].location)};
       });

   _builtins["front"] =
       cell_c([this, load](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "front command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto target = load(cells[1], env);

          if (!(target.type == cell_type_e::LIST)) {
             throw runtime_exception_c("Expected list parameter for front",
                                       cells[1].location);
          }

          if (target.list.empty()) {
             return {CELL_NIL};
          }

          return {target.list[0]};
       });

   _builtins["back"] =
       cell_c([this, load](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "back command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto target = load(cells[1], env);

          if (!(target.type == cell_type_e::LIST)) {
             throw runtime_exception_c("Expected list parameter for back",
                                       cells[1].location);
          }

          if (target.list.empty()) {
             return {CELL_NIL};
          }

          return {*(target.list.end() - 1)};
       });

   _builtins["at"] = cell_c([this, load](std::vector<cell_c> &cells,
                                         std::shared_ptr<environment_c> env)
                                -> std::optional<cell_c> {
      if (cells.size() != 3) {
         throw runtime_exception_c("at command expects 2 parameters, but " +
                                       std::to_string(cells.size() - 1) +
                                       " were given",
                                   cells[0].location);
      }

      auto index = load(cells[1], env);
      if (index.type != cell_type_e::INTEGER) {
         throw runtime_exception_c("at command index must me an integer type",
                                   cells[0].location);
      }

      uint64_t idx = std::stoull(index.data);

      if (cells[2].type != cell_type_e::SYMBOL) {
         throw runtime_exception_c("Second parameter of at must be a variable",
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

   _builtins["clear"] = cell_c([this, load](std::vector<cell_c> &cells,
                                            std::shared_ptr<environment_c> env)
                                   -> std::optional<cell_c> {
      if (cells.size() != 2) {
         throw runtime_exception_c("clear command expects 1 parameters, but " +
                                       std::to_string(cells.size() - 1) +
                                       " were given",
                                   cells[0].location);
      }

      if (cells[1].type != cell_type_e::SYMBOL) {
         throw runtime_exception_c(
             "First parameter of clear must be a variable", cells[1].location);
      }

      auto &variable_name = cells[1].data;

      // If this isn't found it will throw :)
      auto containing_env = env->find(variable_name, cells[1].location);
      cell_c &target = containing_env->get(variable_name);
      target.list.clear();
      return {CELL_TRUE};
   });

   _builtins["pop"] = cell_c([this, load](std::vector<cell_c> &cells,
                                          std::shared_ptr<environment_c> env)
                                 -> std::optional<cell_c> {
      if (cells.size() != 2) {
         throw runtime_exception_c("pop command expects 1 parameters, but " +
                                       std::to_string(cells.size() - 1) +
                                       " were given",
                                   cells[0].location);
      }

      if (cells[1].type != cell_type_e::SYMBOL) {
         throw runtime_exception_c("First parameter of pop must be a variable",
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

   _builtins["push"] = cell_c([this, load](std::vector<cell_c> &cells,
                                           std::shared_ptr<environment_c> env)
                                  -> std::optional<cell_c> {
      if (cells.size() != 3) {
         throw runtime_exception_c("push command expects 2 parameters, but " +
                                       std::to_string(cells.size() - 1) +
                                       " were given",
                                   cells[0].location);
      }

      if (cells[1].type != cell_type_e::SYMBOL) {
         throw runtime_exception_c("First parameter of push must be a variable",
                                   cells[1].location);
      }

      auto &variable_name = cells[1].data;

      // If this isn't found it will throw :)
      auto containing_env = env->find(variable_name, cells[1].location);
      auto value = load(cells[2], env);

      if (value.type == cell_type_e::SYMBOL) {
         throw runtime_exception_c("Expected list or datum value (push)",
                                   cells[2].location);
      }

      cell_c &target = containing_env->get(variable_name);
      target.list.push_back(value);
      return {CELL_TRUE};
   });

   _builtins["not"] = cell_c([this, load](std::vector<cell_c> &cells,
                                          std::shared_ptr<environment_c> env)
                                 -> std::optional<cell_c> {
      if (cells.size() != 2) {
         throw runtime_exception_c("not command expects 1 parameters, but " +
                                       std::to_string(cells.size() - 1) +
                                       " were given",
                                   cells[0].location);
      }

      auto target = load(cells[1], env);

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

   _builtins["assert"] = cell_c([this, load](std::vector<cell_c> &cells,
                                             std::shared_ptr<environment_c> env)
                                    -> std::optional<cell_c> {
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

         auto result = load((*c), env);

         if (static_cast<int>(result.type) <
             static_cast<int>(cell_type_e::STRING)) {
            throw runtime_exception_c(
                "assertion condition did not evaluate to a direectly "
                "comparable type (string, int, double)",
                (*c).location);
         }

         if (cell_type_e::STRING == result.type && result.data.empty()) {
            throw assertion_exception_c("assertion failure: " + cells[1].data,
                                        (*c).location);
         } else if (cell_type_e::INTEGER == result.type &&
                    std::stoull(result.data) < 1) {
            throw assertion_exception_c("assertion failure: " + cells[1].data,
                                        (*c).location);
         } else if (cell_type_e::DOUBLE == result.type &&
                    std::stod(result.data) <= 0.0) {
            throw assertion_exception_c("assertion failure: " + cells[1].data,
                                        (*c).location);
         }
      }
      return {CELL_TRUE};
   });

   _builtins["var"] =
       cell_c([this, load](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          if (cells.size() < 2) {
             throw runtime_exception_c("Nothing given to var command",
                                       cells[0].location);
          }

          auto &variable_name = cells[1].data;

          if (variable_name.find('.') != std::string::npos) {
             throw runtime_exception_c("Attempting to directly define a variable accessor " +
                                           variable_name,
                                       cells[1].location);
          }

          if (_key_symbols.contains(variable_name)) {
             throw runtime_exception_c("Attempting to define a key symbol: " +
                                           variable_name,
                                       cells[1].location);
          }

          if (cells.size() == 2) {
             env->set(variable_name, cell_c(cell_type_e::LIST, "<list>"));
             return {env->get(variable_name)};
          }

          if (cells.size() != 3) {
             throw runtime_exception_c(
                 "var command expects 2 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto value = load(cells[2], env);

          if (value.type == cell_type_e::SYMBOL) {
             throw runtime_exception_c("Expected list or datum value (var)",
                                       cells[2].location);
          }

          env->set(variable_name, value);
          return {env->get(variable_name)};
       });

   _builtins["put"] =
       cell_c([this, load](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          for (auto i = cells.begin() + 1; i != cells.end(); ++i) {

             auto item = load((*i), env);

             std::string stringed;
             cell_to_string(stringed, item, env, false);
             std::cout << stringed;
          }

          return {};
       });

   _builtins["putln"] =
       cell_c([this, load](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          for (auto i = cells.begin() + 1; i != cells.end(); ++i) {

             auto item = load((*i), env);

             std::string stringed;
             cell_to_string(stringed, item, env, false);
             std::cout << stringed;
          }
          std::cout << std::endl;

          return {};
       });

   _builtins["lambda"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          // First item following lambda must be a list of parameters
          std::vector<cell_c> body(cells.begin() + 1, cells.end());
          cell_c lambda(body);
          lambda.type = cell_type_e::LAMBDA;
          lambda.data = cells[1].data;
          return {lambda};
       });

   _builtins["loop"] =
       cell_c([this, load](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
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
             auto conditional_result = load(conditional_cell, loop_environment);

             if (!eval_truthy(conditional_result, cells[0].location)) {
                break;
             }

             // Execute the body
             auto r = process_cell(*body, loop_environment);
             if (r.has_value() && (*r).stop_processing) {
                break;
             }

             if (post) {
                process_cell(*post, loop_environment);
             }
          }

          return {CELL_NIL};
       });

   _builtins["set"] =
       cell_c([this, load](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          if (cells.size() != 3) {
             throw runtime_exception_c(
                 "set command expects 2 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto &variable_name = cells[1].data;

          if (variable_name.find('.') != std::string::npos) {
             std::cout << "processor_builtins::set - Attempting to set an object member - NOT YET COMPLETE\n";
             std::exit(1); 
          }

          // If this isn't found it will throw :)
          auto containing_env = env->find(variable_name, cells[1].location);
          auto value = load(cells[2], env);

          if (value.type == cell_type_e::SYMBOL) {
             throw runtime_exception_c("Expected list or datum value (set)",
                                       cells[2].location);
          }

          containing_env->set(variable_name, value);
          return {containing_env->get(variable_name)};
       });

   // List and block are extremely similar, and realistically `list` coult be
   // used instead of `block` but its "less efficient" as it does the work to
   // construct what would be a temporary cell, while `block` does not.
   _builtins["block"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          for (size_t i = 1; i < cells.size() - 1; i++) {
             auto r = process_cell(cells[i], env);
             if (r.has_value() && (*r).stop_processing) {
                // We want to pass the stop flag up
                return r;
             }
          }

          return process_cell(cells[cells.size() - 1], env);
       });

   _builtins["list"] =
       cell_c([this, load](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          std::vector<cell_c> body;

          for (auto i = cells.begin() + 1; i != cells.end(); ++i) {
             body.push_back(load(*i, env));
          }

          cell_c list(body);
          list.type = cell_type_e::LIST;
          list.data = "<list>";
          return {list};
       });

   _builtins["compose"] =
       cell_c([this](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "compose command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          std::string value;
          for(auto c = cells.begin() + 1; c < cells.end(); c++) {
            quote_cell(value, (*c), env);
          }

          return cell_c(cell_type_e::STRING, value);
       });

   _builtins["decompose"] =
       cell_c([this, load](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "decompose command expects 1 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto target = load(cells[1], env);

          std::optional<cell_c> result;
          eval_c evaluator(env, [&result](std::optional<cell_c> cell) {
               result = cell;
          });

          evaluator.eval(cells[1].location.line, target.data);
          return result;
       });


   _builtins["object"] =
       cell_c([this, load](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          if (cells.size()  != 3) {
             throw runtime_exception_c("object command expectes 3 parameters, but " + 
                                       std::to_string(cells.size() - 1) + " were given",
                                       cells[0].location);
          }

          auto &variable_name = cells[1].data;

          if (_key_symbols.contains(variable_name)) {
             throw runtime_exception_c("Attempting to define a key symbol: " +
                                           variable_name,
                                       cells[1].location);
          }

          // TODO:
          //
          //   Create a new cell of type OBJECT and create an ENV for it. 
          //
          //   run process_cell on the last cell of the given command 
          //   with the new environment that we made
          // 
          //   return the object that we created



          return {};
       });

   _builtins["len"] =
       cell_c([this, load](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          if (cells.size() != 2) {
             throw runtime_exception_c(
                 "len command expects 2 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          return {cell_c(cell_type_e::INTEGER,
                         std::to_string(load(cells[1], env).list.size()),
                         cells[1].location)};
       });

   _builtins["if"] =
       cell_c([this, load](
                  std::vector<cell_c> &cells,
                  std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          if (cells.size() != 3 && cells.size() != 4) {
             throw runtime_exception_c(
                 "if command expects 2-3 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
          }

          auto eval = load(cells[1], env);

          if (eval_truthy(eval, cells[0].location)) {
             return process_cell(cells[2], env);
          } else if (cells.size() == 4) {
             return process_cell(cells[3], env);
          }
          return {};
       });

   _builtins["seq"] = cell_c([this, load](std::vector<cell_c> &cells,
                                          std::shared_ptr<environment_c> env)
                                 -> std::optional<cell_c> {
      if (cells.size() != 3) {
         throw runtime_exception_c("seq command expects 3 parameters, but " +
                                       std::to_string(cells.size() - 1) +
                                       " were given",
                                   cells[0].location);
      }

      auto lhs = load(cells[1], env);
      auto rhs = load(cells[2], env);

      return cell_c(cell_type_e::INTEGER,
                    std::to_string((lhs.data == rhs.data)), cells[0].location);
   });

   _builtins["sneq"] = cell_c([this, load](std::vector<cell_c> &cells,
                                           std::shared_ptr<environment_c> env)
                                  -> std::optional<cell_c> {
      if (cells.size() != 3) {
         throw runtime_exception_c("sneq command expects 3 parameters, but " +
                                       std::to_string(cells.size() - 1) +
                                       " were given",
                                   cells[0].location);
      }

      auto lhs = load(cells[1], env);
      auto rhs = load(cells[2], env);

      return cell_c(cell_type_e::INTEGER,
                    std::to_string((lhs.data != rhs.data)), cells[0].location);
   });

   _builtins["<"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          return {perform_arithmetic(
              "<", cells,
              [](double lhs, double rhs) -> double { return lhs < rhs; }, env)};
       });

   _builtins["<="] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          return {perform_arithmetic(
              "<=", cells,
              [](double lhs, double rhs) -> double { return lhs <= rhs; },
              env)};
       });

   _builtins[">"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          return {perform_arithmetic(
              ">", cells,
              [](double lhs, double rhs) -> double { return lhs > rhs; }, env)};
       });

   _builtins[">="] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          return {perform_arithmetic(
              ">=", cells,
              [](double lhs, double rhs) -> double { return lhs >= rhs; },
              env)};
       });

   _builtins["=="] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          return {perform_arithmetic(
              "==", cells,
              [](double lhs, double rhs) -> double { return lhs == rhs; },
              env)};
       });

   _builtins["!="] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          return {perform_arithmetic(
              "!=", cells,
              [](double lhs, double rhs) -> double { return lhs != rhs; },
              env)};
       });

   _builtins["+"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          return {perform_arithmetic(
              "+", cells,
              [](double lhs, double rhs) -> double { return lhs + rhs; }, env)};
       });

   _builtins["-"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          return {perform_arithmetic(
              "-", cells,
              [](double lhs, double rhs) -> double { return lhs - rhs; }, env)};
       });

   _builtins["/"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
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

   _builtins["*"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          return {perform_arithmetic(
              "*", cells,
              [](double lhs, double rhs) -> double { return lhs * rhs; }, env)};
       });

   _builtins["%"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          return {perform_arithmetic(
              "%", cells,
              [](double lhs, double rhs) -> double {
                 return static_cast<int64_t>(lhs) % static_cast<int64_t>(rhs);
              },
              env)};
       });

   _builtins["or"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
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

   _builtins["and"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
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

   _builtins["xor"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
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