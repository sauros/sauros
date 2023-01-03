#include "list_ext.hpp"

#include <sauros/capi/capi.hpp>
#include <string>

sauros::cell_ptr
_pkg_list_ext_make_assigned_(sauros::cells_t &cells,
                             std::shared_ptr<sauros::environment_c> env) {
   auto value = c_api_process_cell(cells[1], env);
   auto size = c_api_process_cell(cells[2], env);

   if (size->type != sauros::cell_type_e::INTEGER) {
      throw sauros::processor_c::runtime_exception_c(
          "size parameter expected to be of type integer", cells[2]);
   }

   sauros::cell_ptr result =
       std::make_shared<sauros::cell_c>(sauros::cell_type_e::LIST);
   result->list.assign(std::stoull(size->data), value);
   return result;
}

sauros::cell_ptr
_pkg_list_ext_sort_(sauros::cells_t &cells,
                    std::shared_ptr<sauros::environment_c> env) {
   auto data = c_api_process_cell(cells[1], env);

   if (data->type != sauros::cell_type_e::LIST) {
      throw sauros::processor_c::runtime_exception_c(
          "data parameter expected to be of type list", cells[1]);
   }

   return std::make_shared<sauros::cell_c>(sauros::CELL_NIL);
}
