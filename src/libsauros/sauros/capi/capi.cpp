#include "capi.hpp"
#include "sauros/processor/processor.hpp"

sauros::cell_ptr
c_api_process_cell(sauros::cell_ptr cell,
                   std::shared_ptr<sauros::environment_c> env) {
   sauros::processor_c processor;
   return processor.process_cell(cell, env);
}

sauros::cell_ptr
c_api_cell_to_string(sauros::cell_ptr cell,
                     std::shared_ptr<sauros::environment_c> env) {
   sauros::processor_c processor;
   std::string result_string;
   ;
   processor.cell_to_string(result_string, cell, env, false);
   return std::shared_ptr<sauros::cell_c>(
       new sauros::cell_c(sauros::cell_type_e::STRING, result_string));
}
