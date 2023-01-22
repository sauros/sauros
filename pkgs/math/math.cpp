#include "math.hpp"

#include <iostream>
#include <sauros/capi/capi.hpp>
#include <string>

sauros::cell_ptr single_arithmetic(sauros::cells_t &cells,
                                   std::function<double(double)> fn,
                                   std::shared_ptr<sauros::environment_c> env) {
   if (cells.size() < 2) {
      throw sauros::processor_c::runtime_exception_c(
          "given math function expects 1 parameter, but " +
              std::to_string(cells.size() - 1) + " were given",
          cells[0]);
   }

   auto op = [=](sauros::cell_ptr cell,
                 std::shared_ptr<sauros::environment_c> env) -> double {
      auto item = c_api_process_cell(cell, env);
      if (item->type != sauros::cell_type_e::REAL &&
          item->type != sauros::cell_type_e::INTEGER) {
         throw sauros::processor_c::runtime_exception_c(
             "math operation expects parameter to be an integer or a double",
             cell);
      }

      double item_d = item->data.d;
      if (item->type == sauros::cell_type_e::INTEGER) {
         item_d = item->data.i;
      }

      return fn(item_d);
   };

   if (cells.size() == 2) {
      return std::make_shared<sauros::cell_c>(
          sauros::cell_type_e::REAL, std::to_string(op(cells[1], env)));
   }

   auto result = std::make_shared<sauros::cell_c>(sauros::cell_type_e::LIST);
   for (auto c = cells.begin() + 1; c < cells.end(); ++c) {
      result->list.push_back(std::make_shared<sauros::cell_c>(
          sauros::cell_type_e::REAL, std::to_string(op((*c), env))));
   }
   return result;
}

sauros::cell_ptr
_sauros_pkg_math_log_(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env) {
   return single_arithmetic(
       cells, [](double n) -> double { return log(n); }, env);
}

sauros::cell_ptr
_sauros_pkg_math_log2_(sauros::cells_t &cells,
                       std::shared_ptr<sauros::environment_c> env) {
   return single_arithmetic(
       cells, [](double n) -> double { return log2(n); }, env);
}

sauros::cell_ptr
_sauros_pkg_math_log10_(sauros::cells_t &cells,
                        std::shared_ptr<sauros::environment_c> env) {
   return single_arithmetic(
       cells, [](double n) -> double { return log10(n); }, env);
}

sauros::cell_ptr
_sauros_pkg_math_sin_(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env) {
   return single_arithmetic(
       cells, [](double n) -> double { return sin(n); }, env);
}

sauros::cell_ptr
_sauros_pkg_math_cos_(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env) {
   return single_arithmetic(
       cells, [](double n) -> double { return cos(n); }, env);
}

sauros::cell_ptr
_sauros_pkg_math_tan_(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env) {
   return single_arithmetic(
       cells, [](double n) -> double { return tan(n); }, env);
}

sauros::cell_ptr
_sauros_pkg_math_sinh_(sauros::cells_t &cells,
                       std::shared_ptr<sauros::environment_c> env) {
   return single_arithmetic(
       cells, [](double n) -> double { return sinh(n); }, env);
}

sauros::cell_ptr
_sauros_pkg_math_cosh_(sauros::cells_t &cells,
                       std::shared_ptr<sauros::environment_c> env) {
   return single_arithmetic(
       cells, [](double n) -> double { return cosh(n); }, env);
}

sauros::cell_ptr
_sauros_pkg_math_tanh_(sauros::cells_t &cells,
                       std::shared_ptr<sauros::environment_c> env) {
   return single_arithmetic(
       cells, [](double n) -> double { return tanh(n); }, env);
}

sauros::cell_ptr
_sauros_pkg_math_asin_(sauros::cells_t &cells,
                       std::shared_ptr<sauros::environment_c> env) {
   return single_arithmetic(
       cells, [](double n) -> double { return asin(n); }, env);
}

sauros::cell_ptr
_sauros_pkg_math_acos_(sauros::cells_t &cells,
                       std::shared_ptr<sauros::environment_c> env) {
   return single_arithmetic(
       cells, [](double n) -> double { return acos(n); }, env);
}

sauros::cell_ptr
_sauros_pkg_math_atan_(sauros::cells_t &cells,
                       std::shared_ptr<sauros::environment_c> env) {
   return single_arithmetic(
       cells, [](double n) -> double { return atan(n); }, env);
}

sauros::cell_ptr
_sauros_pkg_math_asinh_(sauros::cells_t &cells,
                        std::shared_ptr<sauros::environment_c> env) {
   return single_arithmetic(
       cells, [](double n) -> double { return asinh(n); }, env);
}

sauros::cell_ptr
_sauros_pkg_math_acosh_(sauros::cells_t &cells,
                        std::shared_ptr<sauros::environment_c> env) {
   return single_arithmetic(
       cells, [](double n) -> double { return acosh(n); }, env);
}

sauros::cell_ptr
_sauros_pkg_math_atanh_(sauros::cells_t &cells,
                        std::shared_ptr<sauros::environment_c> env) {
   return single_arithmetic(
       cells, [](double n) -> double { return atanh(n); }, env);
}

sauros::cell_ptr
_sauros_pkg_math_exp_(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env) {
   return single_arithmetic(
       cells, [](double n) -> double { return exp(n); }, env);
}

sauros::cell_ptr
_sauros_pkg_math_sqrt_(sauros::cells_t &cells,
                       std::shared_ptr<sauros::environment_c> env) {
   return single_arithmetic(
       cells, [](double n) -> double { return sqrt(n); }, env);
}

sauros::cell_ptr
_sauros_pkg_math_ceil_(sauros::cells_t &cells,
                       std::shared_ptr<sauros::environment_c> env) {
   return single_arithmetic(
       cells, [](double n) -> double { return ceil(n); }, env);
}

sauros::cell_ptr
_sauros_pkg_math_floor_(sauros::cells_t &cells,
                        std::shared_ptr<sauros::environment_c> env) {
   return single_arithmetic(
       cells, [](double n) -> double { return floor(n); }, env);
}

sauros::cell_ptr
_sauros_pkg_math_abs_(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env) {
   return single_arithmetic(
       cells, [](double n) -> double { return fabs(n); }, env);
}

sauros::cell_ptr
_sauros_pkg_math_pow_(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env) {
   if (cells.size() != 3) {
      throw sauros::processor_c::runtime_exception_c(
          "given math function expects 3 parameter, but " +
              std::to_string(cells.size()) + " were given",
          cells[0]);
   }

   auto op = [=](sauros::cell_ptr &lhs_, sauros::cell_ptr &rhs_,
                 std::shared_ptr<sauros::environment_c> env) -> double {
      auto lhs = c_api_process_cell(lhs_, env);
      if (lhs->type != sauros::cell_type_e::REAL &&
          lhs->type != sauros::cell_type_e::INTEGER) {
         throw sauros::processor_c::runtime_exception_c(
             "math operation expects parameter to be an integer or a double",
             lhs);
      }

      auto rhs = c_api_process_cell(rhs_, env);
      if (rhs->type != sauros::cell_type_e::REAL &&
          rhs->type != sauros::cell_type_e::INTEGER) {
         throw sauros::processor_c::runtime_exception_c(
             "math operation expects parameter to be an integer or a double",
             rhs);
      }

      double lhs_d = lhs->data.d;
      double rhs_d = rhs->data.d;
      if (lhs->type == sauros::cell_type_e::INTEGER) {
         lhs_d = lhs->data.i;
      }
      if (rhs->type == sauros::cell_type_e::INTEGER) {
         rhs_d = rhs->data.i;
      }
      return pow(lhs_d, rhs_d);
   };

   return std::make_shared<sauros::cell_c>(
       sauros::cell_type_e::REAL, std::to_string(op(cells[1], cells[2], env)));
}