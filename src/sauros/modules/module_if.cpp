#include "module_if.hpp"
#include "sauros/processor/processor.hpp"

namespace sauros {

cell_c module_if::load(cell_c &cell, std::shared_ptr<environment_c> env) {
   processor_c processor;
   auto target = processor.process_cell(cell, env);
   if (!target.has_value()) {
      throw processor_c::runtime_exception_c("Unable to process value",
                                             cell.location);
   }
   return (*target);
}

}