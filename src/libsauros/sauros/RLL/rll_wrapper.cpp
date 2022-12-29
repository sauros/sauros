#include "rll_wrapper.hpp"

#include "RLL.h"

namespace sauros {

rll_wrapper_c::rll_wrapper_c() : _lib(new rll::shared_library()) {}

void rll_wrapper_c::load(std::string target) {
   try {
      _lib->load(target);
   } catch (rll::exception::library_loading_error &e) {
      throw library_loading_error_c(e.what());
   }
}

bool rll_wrapper_c::is_loaded() { return _lib->is_loaded(); }

bool rll_wrapper_c::has_symbol(std::string symbol) {
   return _lib->has_symbol(symbol);
}

void *rll_wrapper_c::get_symbol(std::string symbol) {
   return _lib->get_symbol(symbol);
}

std::shared_ptr<rll::shared_library> make_rll_lib() {
   return std::shared_ptr<rll::shared_library>(new rll::shared_library());
}

} // namespace sauros