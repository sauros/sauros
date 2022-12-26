#include "capi.hpp"
#include "sauros/processor/processor.hpp"

sauros::cell_ptr
c_api_process_cell(sauros::cell_ptr cell, 
             std::shared_ptr<sauros::environment_c> env) {
   sauros::processor_c processor;
   return processor.process_cell(cell, env);
}
