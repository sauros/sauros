#include "processor.hpp"
#include "sauros/profiler.hpp"
#include <iostream>

namespace sauros {

cell_ptr processor_c::perform_arithmetic(
    std::string op, cells_t &cells, std::function<double(double, double)> fn,
    std::shared_ptr<environment_c> env, bool force_double) {
#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("environment_c::perform_arithmetic");
#endif

   if (cells.size() < 3) {
      throw runtime_exception_c("Expected a list size of at least 3 items",
                                cells[0]);
   }

   /*
      double result = 0.0;
      bool store_as_double{false};
      auto first_cell_value = process_cell(cells[1], env);
      try {
         result = std::stod(first_cell_value->data);
      } catch (const std::invalid_argument &) {
         throw runtime_exception_c("Invalid data type given for operand",
                                   cells[0]);
      } catch (const std::out_of_range &) {
         throw runtime_exception_c("Item caused out of range exception",
      cells[0]);
      }
   */

   bool store_as_double{false};
   auto first_cell_value = process_cell(cells[1], env);

   double result = first_cell_value->data.d;
   if (first_cell_value->type == cell_type_e::INTEGER) {
      result = first_cell_value->data.i;
   }

   for (auto i = cells.begin() + 2; i != cells.end(); ++i) {

      auto cell_value = process_cell((*i), env);
      store_as_double = static_cast<unsigned short>(cell_value->type) == 0;

      /*
      try {
         result = fn(result, std::stod(cell_value->data));
      } catch (const std::invalid_argument &) {
         throw runtime_exception_c("Invalid data type given for operand",
                                   cells[0]);
      } catch (const std::out_of_range &) {
         throw runtime_exception_c("Item caused out of range exception",
                                   cells[0]);
      }
      */

      double cell_value_actual = cell_value->data.d;
      if (cell_value->type == cell_type_e::INTEGER) {
         cell_value_actual = cell_value->data.i;
      }
      result = fn(result, cell_value_actual);
   }

   return (force_double || store_as_double)
              ? std::make_shared<cell_c>(cell_type_e::REAL, result,
                                         cells[0]->location)
              : std::make_shared<cell_c>(cell_type_e::INTEGER,
                                         static_cast<cell_int_t>(result),
                                         cells[0]->location);
}

} // namespace sauros