#include "math.hpp"

#include <iostream>
#include <math.h>
#include <string>

#include "sauros/processor/processor.hpp"

namespace sauros {
namespace modules {

math_c::math_c() {
   auto single_arithmetic =
       [this](cells_t &cells, std::function<double(double)> fn,
              std::shared_ptr<environment_c> env) -> cell_ptr {
      if (cells.size() < 2) {
         throw processor_c::runtime_exception_c(
             "given math function expects 1 parameter, but " +
                 std::to_string(cells.size() - 1) + " were given",
             cells[0]->location);
      }

      auto op = [this, fn](cell_ptr cell,
                           std::shared_ptr<environment_c> env) -> double {
         auto item = load(cell, env);
         if (item->type != cell_type_e::DOUBLE &&
             item->type != cell_type_e::INTEGER) {
            throw processor_c::runtime_exception_c(
                "math operation expects parameter to be an integer or a double",
                cell->location);
         }

         return fn(std::stod(item->data));
      };

      if (cells.size() == 2) {
         return std::make_shared<cell_c>(cell_type_e::DOUBLE, std::to_string(op(cells[1], env)));
      }

      auto result = std::make_shared<cell_c>(cell_type_e::LIST);
      for (auto c = cells.begin() + 1; c < cells.end(); ++c) {
         result->list.push_back(
             std::make_shared<cell_c>(cell_type_e::DOUBLE, std::to_string(op((*c), env))));
      }
      return result;
   };

   _members_map["pi"] = std::make_shared<cell_c>(cell_type_e::DOUBLE, "3.14159265359");
   _members_map["phi"] = std::make_shared<cell_c>(cell_type_e::DOUBLE, "1.61803398875");
   _members_map["e"] = std::make_shared<cell_c>(cell_type_e::DOUBLE, "2.71828182845");

   _members_map["log"] = std::make_shared<cell_c>(
       [this, single_arithmetic](cells_t &cells,
                                 std::shared_ptr<environment_c> env) -> cell_ptr {
          return single_arithmetic(
              cells, [](double n) -> double { return log(n); }, env);
       });

   _members_map["log2"] = std::make_shared<cell_c>(
       [this, single_arithmetic](cells_t &cells,
                                 std::shared_ptr<environment_c> env) -> cell_ptr {
          return single_arithmetic(
              cells, [](double n) -> double { return log2(n); }, env);
       });

   _members_map["log10"] = std::make_shared<cell_c>(
       [this, single_arithmetic](cells_t &cells,
                                 std::shared_ptr<environment_c> env) -> cell_ptr {
          return single_arithmetic(
              cells, [](double n) -> double { return log10(n); }, env);
       });

   _members_map["sin"] = std::make_shared<cell_c>(
       [this, single_arithmetic](cells_t &cells,
                                 std::shared_ptr<environment_c> env) -> cell_ptr {
          return single_arithmetic(
              cells, [](double n) -> double { return sin(n); }, env);
       });

   _members_map["cos"] = std::make_shared<cell_c>(
       [this, single_arithmetic](cells_t &cells,
                                 std::shared_ptr<environment_c> env) -> cell_ptr {
          return single_arithmetic(
              cells, [](double n) -> double { return cos(n); }, env);
       });

   _members_map["tan"] = std::make_shared<cell_c>(
       [this, single_arithmetic](cells_t &cells,
                                 std::shared_ptr<environment_c> env) -> cell_ptr {
          return single_arithmetic(
              cells, [](double n) -> double { return tan(n); }, env);
       });

   _members_map["asin"] = std::make_shared<cell_c>(
       [this, single_arithmetic](cells_t &cells,
                                 std::shared_ptr<environment_c> env) -> cell_ptr {
          return single_arithmetic(
              cells, [](double n) -> double { return asin(n); }, env);
       });

   _members_map["acos"] = std::make_shared<cell_c>(
       [this, single_arithmetic](cells_t &cells,
                                 std::shared_ptr<environment_c> env) -> cell_ptr {
          return single_arithmetic(
              cells, [](double n) -> double { return acos(n); }, env);
       });

   _members_map["atan"] = std::make_shared<cell_c>(
       [this, single_arithmetic](cells_t &cells,
                                 std::shared_ptr<environment_c> env) -> cell_ptr {
          return single_arithmetic(
              cells, [](double n) -> double { return atan(n); }, env);
       });

   _members_map["sinh"] = std::make_shared<cell_c>(
       [this, single_arithmetic](cells_t &cells,
                                 std::shared_ptr<environment_c> env) -> cell_ptr {
          return single_arithmetic(
              cells, [](double n) -> double { return sinh(n); }, env);
       });

   _members_map["cosh"] = std::make_shared<cell_c>(
       [this, single_arithmetic](cells_t &cells,
                                 std::shared_ptr<environment_c> env) -> cell_ptr {
          return single_arithmetic(
              cells, [](double n) -> double { return cosh(n); }, env);
       });

   _members_map["tanh"] = std::make_shared<cell_c>(
       [this, single_arithmetic](cells_t &cells,
                                 std::shared_ptr<environment_c> env) -> cell_ptr {
          return single_arithmetic(
              cells, [](double n) -> double { return tanh(n); }, env);
       });

   _members_map["asinh"] = std::make_shared<cell_c>(
       [this, single_arithmetic](cells_t &cells,
                                 std::shared_ptr<environment_c> env) -> cell_ptr {
          return single_arithmetic(
              cells, [](double n) -> double { return asinh(n); }, env);
       });

   _members_map["acosh"] = std::make_shared<cell_c>(
       [this, single_arithmetic](cells_t &cells,
                                 std::shared_ptr<environment_c> env) -> cell_ptr {
          return single_arithmetic(
              cells, [](double n) -> double { return acosh(n); }, env);
       });

   _members_map["atanh"] = std::make_shared<cell_c>(
       [this, single_arithmetic](cells_t &cells,
                                 std::shared_ptr<environment_c> env) -> cell_ptr {
          return single_arithmetic(
              cells, [](double n) -> double { return atanh(n); }, env);
       });

   _members_map["exp"] = std::make_shared<cell_c>(
       [this, single_arithmetic](cells_t &cells,
                                 std::shared_ptr<environment_c> env) -> cell_ptr {
          return single_arithmetic(
              cells, [](double n) -> double { return exp(n); }, env);
       });

   _members_map["sqrt"] = std::make_shared<cell_c>(
       [this, single_arithmetic](cells_t &cells,
                                 std::shared_ptr<environment_c> env) -> cell_ptr {
          return single_arithmetic(
              cells, [](double n) -> double { return sqrt(n); }, env);
       });

   _members_map["ceil"] = std::make_shared<cell_c>(
       [this, single_arithmetic](cells_t &cells,
                                 std::shared_ptr<environment_c> env) -> cell_ptr {
          return single_arithmetic(
              cells, [](double n) -> double { return ceil(n); }, env);
       });

   _members_map["floor"] = std::make_shared<cell_c>(
       [this, single_arithmetic](cells_t &cells,
                                 std::shared_ptr<environment_c> env) -> cell_ptr {
          return single_arithmetic(
              cells, [](double n) -> double { return floor(n); }, env);
       });

   _members_map["abs"] = std::make_shared<cell_c>(
       [this, single_arithmetic](cells_t &cells,
                                 std::shared_ptr<environment_c> env) -> cell_ptr {
          return single_arithmetic(
              cells, [](double n) -> double { return fabs(n); }, env);
       });

   _members_map["pow"] = std::make_shared<cell_c>([this](cells_t &cells,
                                       std::shared_ptr<environment_c> env)
                                    -> cell_ptr {
      if (cells.size() != 3) {
         throw processor_c::runtime_exception_c(
             "given math function expects 3 parameter, but " +
                 std::to_string(cells.size()) + " were given",
             cells[0]->location);
      }

      auto op = [this](cell_ptr &lhs_, cell_ptr &rhs_,
                       std::shared_ptr<environment_c> env) -> double {
         auto lhs = load(lhs_, env);
         if (lhs->type != cell_type_e::DOUBLE &&
             lhs->type != cell_type_e::INTEGER) {
            throw processor_c::runtime_exception_c(
                "math operation expects parameter to be an integer or a double",
                lhs->location);
         }

         auto rhs = load(rhs_, env);
         if (rhs->type != cell_type_e::DOUBLE &&
             rhs->type != cell_type_e::INTEGER) {
            throw processor_c::runtime_exception_c(
                "math operation expects parameter to be an integer or a double",
                rhs->location);
         }
         return pow(std::stod(lhs->data), std::stod(rhs->data));
      };

      return std::make_shared<cell_c>(cell_type_e::DOUBLE,
                    std::to_string(op(cells[1], cells[2], env)));
   });
}

} // namespace modules
} // namespace sauros