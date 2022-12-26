#include "module_if.hpp"
#include "processor/processor.hpp"

namespace sauros {

cell_ptr module_if::load(cell_ptr cell, std::shared_ptr<environment_c> env) {
   processor_c processor;
   return processor.process_cell(cell, env);
}

} // namespace sauros