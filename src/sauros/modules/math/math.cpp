#include "math.hpp"

#include <iostream>
#include <string>
#include <math.h> 

#include "sauros/processor/processor.hpp"

namespace sauros {
namespace modules {

math_c::math_c() {

   auto load = [&](cell_c &cell, std::shared_ptr<environment_c> env) -> cell_c {
      processor_c processor;
      auto target = processor.process_cell(cell, env);
      if (!target.has_value()) {
         throw processor_c::runtime_exception_c("Unable to process value", cell.location);
      }
      return (*target);
   };

   auto single_arithmetic = [load]( std::vector<cell_c> &cells,
                                    std::function<double(double)> fn,
                                    std::shared_ptr<environment_c> env) -> cell_c {
      if (cells.size() < 2) {
         throw processor_c::runtime_exception_c(
            "given math function expects 1 parameter, but " +
                  std::to_string(cells.size() - 1) + " were given",
            cells[0].location);
      }

      auto op = [load, fn](cell_c &cell, std::shared_ptr<environment_c> env) -> double {
         auto item = load(cell, env);
         if (item.type != cell_type_e::DOUBLE && item.type != cell_type_e::INTEGER) {
            throw processor_c::runtime_exception_c(
                  "math operation expects parameter to be an integer or a double",
                  cell.location);
         }

         return fn(std::stod(item.data));
      };

      if (cells.size() == 2) {
         return cell_c(cell_type_e::DOUBLE, std::to_string(op(cells[1], env)));
      }

      cell_c result(cell_type_e::LIST);
      for (auto c = cells.begin() + 1; c < cells.end(); ++c) {
         result.list.push_back(cell_c(cell_type_e::DOUBLE, std::to_string(op((*c), env))));
      }
      return result;
   };

   _members_map["pi"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

         return {sauros::cell_c(sauros::cell_type_e::DOUBLE, "3.14159265359")};
   });

   _members_map["e"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

         if (cells.size() != 1) {
             throw processor_c::runtime_exception_c(
                 "math::e expects 0 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
         }

         return {sauros::cell_c(sauros::cell_type_e::DOUBLE, "2.71828182845")};
   });

   _members_map["phi"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

         if (cells.size() != 1) {
             throw processor_c::runtime_exception_c(
                 "math::phi expects 0 parameters, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0].location);
         }

         return {sauros::cell_c(sauros::cell_type_e::DOUBLE, "1.61803398875")};
   });

   _members_map["log"] = cell_c(
       [this, single_arithmetic](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          return single_arithmetic(cells, [](double n)->double{
            return log(n);
          }, env);
   });

   _members_map["log2"] = cell_c(
       [this, single_arithmetic](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          return single_arithmetic(cells, [](double n)->double{
            return log2(n);
          }, env);
   });

   _members_map["log10"] = cell_c(
       [this, single_arithmetic](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          return single_arithmetic(cells, [](double n)->double{
            return log10(n);
          }, env);
   });

   _members_map["sin"] = cell_c(
       [this, single_arithmetic](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          return single_arithmetic(cells, [](double n)->double{
            return sin(n);
          }, env);
   });

   _members_map["cos"] = cell_c(
       [this, single_arithmetic](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          return single_arithmetic(cells, [](double n)->double{
            return cos(n);
          }, env);
   });

   _members_map["tan"] = cell_c(
       [this, single_arithmetic](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          return single_arithmetic(cells, [](double n)->double{
            return tan(n);
          }, env);
   });

   _members_map["asin"] = cell_c(
       [this, single_arithmetic](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          return single_arithmetic(cells, [](double n)->double{
            return asin(n);
          }, env);
   });

   _members_map["acos"] = cell_c(
       [this, single_arithmetic](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          return single_arithmetic(cells, [](double n)->double{
            return acos(n);
          }, env);
   });

   _members_map["atan"] = cell_c(
       [this, single_arithmetic](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          return single_arithmetic(cells, [](double n)->double{
            return atan(n);
          }, env);
   });

   _members_map["sinh"] = cell_c(
       [this, single_arithmetic](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          return single_arithmetic(cells, [](double n)->double{
            return sinh(n);
          }, env);
   });

   _members_map["cosh"] = cell_c(
       [this, single_arithmetic](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          return single_arithmetic(cells, [](double n)->double{
            return cosh(n);
          }, env);
   });

   _members_map["tanh"] = cell_c(
       [this, single_arithmetic](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          return single_arithmetic(cells, [](double n)->double{
            return tanh(n);
          }, env);
   });

   _members_map["asinh"] = cell_c(
       [this, single_arithmetic](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          return single_arithmetic(cells, [](double n)->double{
            return asinh(n);
          }, env);
   });

   _members_map["acosh"] = cell_c(
       [this, single_arithmetic](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          return single_arithmetic(cells, [](double n)->double{
            return acosh(n);
          }, env);
   });

   _members_map["atanh"] = cell_c(
       [this, single_arithmetic](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          return single_arithmetic(cells, [](double n)->double{
            return atanh(n);
          }, env);
   });

   _members_map["exp"] = cell_c(
       [this, single_arithmetic](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          return single_arithmetic(cells, [](double n)->double{
            return exp(n);
          }, env);
   });

   _members_map["sqrt"] = cell_c(
       [this, single_arithmetic](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          return single_arithmetic(cells, [](double n)->double{
            return sqrt(n);
          }, env);
   });

   _members_map["ceil"] = cell_c(
       [this, single_arithmetic](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          return single_arithmetic(cells, [](double n)->double{
            return ceil(n);
          }, env);
   });

   _members_map["floor"] = cell_c(
       [this, single_arithmetic](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          return single_arithmetic(cells, [](double n)->double{
            return floor(n);
          }, env);
   });

   _members_map["abs"] = cell_c(
       [this, single_arithmetic](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

          return single_arithmetic(cells, [](double n)->double{
            return fabs(n);
          }, env);
   });

   _members_map["pow"] = cell_c(
       [this, load](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {

         if (cells.size() != 3) {
            throw processor_c::runtime_exception_c(
               "given math function expects 3 parameter, but " +
                     std::to_string(cells.size()) + " were given",
               cells[0].location);
         }

         auto op = [load](cell_c &lhs_, cell_c &rhs_, std::shared_ptr<environment_c> env) -> double {
            auto lhs = load(lhs_, env);
            if (lhs.type != cell_type_e::DOUBLE && lhs.type != cell_type_e::INTEGER) {
               throw processor_c::runtime_exception_c(
                     "math operation expects parameter to be an integer or a double",
                     lhs.location);
            }

            auto rhs = load(rhs_, env);
            if (rhs.type != cell_type_e::DOUBLE && rhs.type != cell_type_e::INTEGER) {
               throw processor_c::runtime_exception_c(
                     "math operation expects parameter to be an integer or a double",
                     rhs.location);
            }
            return pow(std::stod(lhs.data), std::stod(rhs.data));
         };

         return cell_c(cell_type_e::DOUBLE, std::to_string(op(cells[1], cells[2], env)));
   });
}

} // namespace modules
} // namespace sauros