#include "module_if.hpp"
#include "sauros/processor/processor.hpp"

namespace sauros {

cell_c module_if::load(cell_c &cell, std::shared_ptr<environment_c> env) {
   processor_c processor;
   return processor.process_cell(cell, env);
}

} // namespace sauros