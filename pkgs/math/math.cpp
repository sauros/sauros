#include "math.hpp"

#include <iostream>
#include <string>
#include <sauros/capi/capi.hpp>

sauros::cell_ptr single_arithmetic(
              sauros::cells_t &cells, std::function<double(double)> fn,
              std::shared_ptr<sauros::environment_c> env) {
   if (cells.size() < 2) {
   throw sauros::processor_c::runtime_exception_c(
         "given math function expects 1 parameter, but " +
            std::to_string(cells.size() - 1) + " were given",
         cells[0]->location);
   }

   auto op = [=](sauros::cell_ptr cell,
                        std::shared_ptr<sauros::environment_c> env) -> double {
                           
      std::cout << "About to process!" << std::endl;
      auto item = c_api_process_cell(cell, env);

      std::cout << "Processed!" << std::endl;
      if (item->type != sauros::cell_type_e::DOUBLE &&
            item->type != sauros::cell_type_e::INTEGER) {
         throw sauros::processor_c::runtime_exception_c(
               "math operation expects parameter to be an integer or a double",
               cell->location);
      }

      return fn(std::stod(item->data));
   };

   if (cells.size() == 2) {
      return std::make_shared<sauros::cell_c>(sauros::cell_type_e::DOUBLE,
                                       std::to_string(op(cells[1], env)));
   }

   auto result = std::make_shared<sauros::cell_c>(sauros::cell_type_e::LIST);
   for (auto c = cells.begin() + 1; c < cells.end(); ++c) {
      result->list.push_back(std::make_shared<sauros::cell_c>(
            sauros::cell_type_e::DOUBLE, std::to_string(op((*c), env))));
   }
   return result;
}


sauros::cell_ptr
_sauros_pkg_math_log_(sauros::cells_t &cells,
                     std::shared_ptr<sauros::environment_c> env) {
   return single_arithmetic(
               cells, [](double n) -> double { return log(n); }, env);
}

