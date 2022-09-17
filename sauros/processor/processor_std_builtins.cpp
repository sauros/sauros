#include "processor.hpp"

#include <iostream>

namespace sauros {

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
   _key_symbols.insert("car");
   _key_symbols.insert("cons");
   _key_symbols.insert("cdr");
   _key_symbols.insert("list");
   _key_symbols.insert("set");
   _key_symbols.insert("lambda");
   _key_symbols.insert("block");
   _key_symbols.insert("len");
   _key_symbols.insert("empty?");
   _key_symbols.insert("put");
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

   _builtins["exit"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          if (cells.size() != 2) {
             throw runtime_exception_c("var command expects 1 parameters, but" +
                                           std::to_string(cells.size() - 1) +
                                           " were given",
                                       cells[0].location);
          }


          auto value = process_cell(cells[1], env);

          if ((*value).type != cell_type_e::INTEGER) {
             throw runtime_exception_c("Expected integer type for exit value",
                                       cells[1].location);
          }

          if (!value.has_value()) {
             throw runtime_exception_c("Unable to locate value for exit",
                                       cells[1].location);
          }

          std::exit(std::stoull((*value).data));
       });

   _builtins["car"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          auto target = process_cell(cells[1], env);
          if (!target.has_value()) {
             throw runtime_exception_c("Unable to process value for car",
                                       cells[1].location);
          }

          if (!((*target).type == cell_type_e::LIST)) {
             throw runtime_exception_c("Expected list parameter for car",
                                       cells[1].location);
          }

          if ((*target).list.empty()) {
             return {CELL_NIL};
          }

          return {(*target).list[0]};
       });

   _builtins["cdr"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          auto target = process_cell(cells[1], env);
          if (!target.has_value()) {
             throw runtime_exception_c("Unable to process value for cdr",
                                       cells[1].location);
          }

          if (!((*target).type == cell_type_e::LIST)) {
             throw runtime_exception_c("Expected list parameter for cdr",
                                       cells[1].location);
          }

          if ((*target).list.empty()) {
             return {CELL_NIL};
          }

          cell_c result((*target));
          result.list.erase(result.list.begin());
          return {result};
       });

   _builtins["cons"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          if (cells.size() != 3) {
             throw runtime_exception_c("set command expects 2 parameters, but" +
                                           std::to_string(cells.size() - 1) +
                                           " were given",
                                       cells[0].location);
          }

          auto load = [&](cell_c &cell) -> cell_c {
             auto target = process_cell(cell, env);
             if (!target.has_value()) {
                throw runtime_exception_c("Unable to process value",
                                          cell.location);
             }
             return (*target);
          };

          auto lhs = load(cells[1]);
          auto rhs = load(cells[2]);

          cell_c result(cell_type_e::LIST, "");
          result.list.push_back(lhs);
          result.list.push_back(rhs);
          return {result};
       });

   _builtins["not"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
         
         if (cells.size() != 2) {
            throw runtime_exception_c("not command expects 1 parameters, but" +
                                          std::to_string(cells.size() - 1) +
                                          " were given",
                                    cells[0].location);
         }

         auto target = process_cell(cells[1], env);
         if (!target.has_value()) {
            throw runtime_exception_c("Unable to process value",
                                    cells[1].location);
         }

         if ((*target).type != cell_type_e::INTEGER && (*target).type != cell_type_e::DOUBLE) {
            throw runtime_exception_c("not command expects parameter to evaluate to a numerical type", cells[1].location);
         }

         if (std::stod((*target).data) > 0.0) {
            return {CELL_FALSE};
         } else {
            return {CELL_TRUE};
         }
       });

   _builtins["assert"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
         
         if (cells.size() < 3) {
            throw runtime_exception_c("assert command expects at least 3 parameters, but" +
                                          std::to_string(cells.size() - 1) +
                                          " were given",
                                    cells[0].location);
         }

         if (cells[1].type != cell_type_e::STRING) {
            throw runtime_exception_c("assertion label bust be a raw string", cells[1].location);
         }

         for (auto c = cells.begin()+2; c != cells.end(); c++) {

            auto result = process_cell((*c), env);

            if (!result.has_value()) {
               throw runtime_exception_c("failed to process assertion condition", (*c).location);
            }

            if ( static_cast<int>((*result).type) < static_cast<int>(cell_type_e::STRING) ) {
               throw runtime_exception_c("assertion condition did not evaluate to a direectly comparable type (string, int, double)", (*c).location);
            }

            if (cell_type_e::STRING == (*result).type && (*result).data.empty()) {
               throw assertion_exception_c(cells[1].data, (*c).location);
            } else if (cell_type_e::INTEGER == (*result).type && std::stoull((*result).data) < 1) {
               throw assertion_exception_c(cells[1].data, (*c).location);
            } else if (cell_type_e::DOUBLE == (*result).type && std::stod((*result).data) <= 0.0) {
               throw assertion_exception_c(cells[1].data, (*c).location);
            }
         }
         return {CELL_TRUE};
       });

   _builtins["empty?"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          if (cells.size() != 2) {
            throw runtime_exception_c("empty? command expects 1 parameters, but" +
                                          std::to_string(cells.size() - 1) +
                                          " were given",
                                    cells[0].location);
          }

          if (cells[1].list.empty())
             return {CELL_TRUE};
          return {CELL_FALSE};
       });

   _builtins["var"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          if (cells.size() != 3) {
             throw runtime_exception_c("var command expects 2 parameters, but" +
                                           std::to_string(cells.size() - 1) +
                                           " were given",
                                       cells[0].location);
          }

          auto &variable_name = cells[1].data;

          if (_key_symbols.contains(variable_name)) {
             throw runtime_exception_c("Attempting to define a key symbol: " +
                                           variable_name,
                                       cells[1].location);
          }

          auto value = process_cell(cells[2], env);

          if ((*value).type == cell_type_e::SYMBOL) {
             throw runtime_exception_c("Expected list or datum value",
                                       cells[2].location);
          }

          if (!value.has_value()) {
             throw runtime_exception_c("Unable to locate value for assignment",
                                       cells[2].location);
          }

          env->set(variable_name, (*value));
          return {env->get(variable_name)};
       });

   _builtins["put"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          for (auto i = cells.begin() + 1; i != cells.end(); ++i) {

             auto item = process_cell((*i), env);

             if (!item.has_value()) {
                throw runtime_exception_c("Unknown operand given to 'put'",
                                          cells[1].location);
             }

             std::string stringed;
             cell_to_string(stringed, (*item), env, false);
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

   _builtins["block"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          // First item following lambda must be a list of parameters

          for (size_t i = 1; i < cells.size() - 1; i++) {
             process_cell(cells[i], env);
          }
          return process_cell(cells[cells.size() - 1], env);
       });

   _builtins["set"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          if (cells.size() != 3) {
             throw runtime_exception_c("set command expects 2 parameters, but" +
                                           std::to_string(cells.size() - 1) +
                                           " were given",
                                       cells[0].location);
          }

          auto &variable_name = cells[1].data;

          // If this isn't found it will throw :)
          { auto _ = env->find(variable_name); }

          auto value = process_cell(cells[2], env);

          if ((*value).type == cell_type_e::SYMBOL) {
             throw runtime_exception_c("Expected list or datum value",
                                       cells[2].location);
          }

          if (!value.has_value()) {
             throw runtime_exception_c("Unable to locate value for assignment",
                                       cells[2].location);
          }

          env->set(variable_name, (*value));
          return {env->get(variable_name)};
       });

   _builtins["list"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          // First item following lambda must be a list of parameters
          std::vector<cell_c> body;

          for (auto i = cells.begin() + 1; i != cells.end(); ++i) {
             body.push_back((*process_cell(*i, env)));
          }

          cell_c list(body);
          list.type = cell_type_e::LIST;
          list.data = "<list>";
          return {list};
       });

   _builtins["len"] = cell_c([this](std::vector<cell_c> &cells,
                                    std::shared_ptr<environment_c> env)
                                 -> std::optional<cell_c> {
      if (cells.size() != 2) {
         throw runtime_exception_c("len command expects 2 parameters, but " +
                                       std::to_string(cells.size() - 1) +
                                       " were given",
                                   cells[0].location);
      }

      return {cell_c(cell_type_e::INTEGER,
                     std::to_string((*process_cell(cells[1], env)).list.size()),
                     cells[1].location)};
   });

   _builtins["if"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          if (cells.size() != 3 && cells.size() != 4) {
             throw runtime_exception_c("if command expects 2-3 parameters, but " +
                                           std::to_string(cells.size() - 1) +
                                           " were given",
                                       cells[0].location);
          }

          auto eval = process_cell(cells[1], env);
          if (!eval.has_value()) {
             throw runtime_exception_c(
                 "Evaluated cell contained no value to evaluate",
                 cells[1].location);
          }

          bool is_true{false};
          switch ((*eval).type) {
          case cell_type_e::STRING:
             is_true = (!(*eval).data.empty());
             break;
          case cell_type_e::LIST:
             is_true = (!(*eval).list.empty());
             break;
          case cell_type_e::LAMBDA:
             is_true = true;
             break;
          case cell_type_e::DOUBLE:
             [[fallthrough]];
          case cell_type_e::INTEGER: {
             try {
                auto v = std::stod((*eval).data);
                is_true = (v > 0.0);
             } catch (const std::invalid_argument &) {
                throw runtime_exception_c("Invalid data type given for operand",
                                          cells[0].location);
             } catch (const std::out_of_range &) {
                throw runtime_exception_c("Item caused out of range exception",
                                          cells[0].location);
             }
             break;
          }
          }

          if (is_true) {
            return process_cell(cells[2], env);
          } else if (cells.size() == 4) {
            return process_cell(cells[3], env);
          }
          return {};
       });

   _builtins["seq"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          if (cells.size() != 3) {
             throw runtime_exception_c("eq command expects 3 parameters, but " +
                                           std::to_string(cells.size() - 1) +
                                           " were given",
                                       cells[0].location);
          }

          auto load = [&](cell_c &cell) -> cell_c {
             auto target = process_cell(cell, env);
             if (!target.has_value()) {
                throw runtime_exception_c("Unable to process value",
                                          cell.location);
             }
             return (*target);
          };

          auto lhs = load(cells[1]);
          auto rhs = load(cells[2]);

          return cell_c(cell_type_e::INTEGER,
                        std::to_string((lhs.data == rhs.data)),
                        cells[0].location);
       });

   _builtins["sneq"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          if (cells.size() != 3) {
             throw runtime_exception_c("eq command expects 3 parameters, but " +
                                           std::to_string(cells.size() - 1) +
                                           " were given",
                                       cells[0].location);
          }


          auto load = [&](cell_c &cell) -> cell_c {
             auto target = process_cell(cell, env);
             if (!target.has_value()) {
                throw runtime_exception_c("Unable to process value",
                                          cell.location);
             }
             return (*target);
          };

          auto lhs = load(cells[1]);
          auto rhs = load(cells[2]);

          return cell_c(cell_type_e::INTEGER,
                        std::to_string((lhs.data != rhs.data)),
                        cells[0].location);
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
}

} // namespace sauros