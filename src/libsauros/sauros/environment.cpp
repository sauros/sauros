#include "environment.hpp"

#include "profiler.hpp"
#include <iostream>

#include <sstream>
#include <vector>

static inline std::vector<std::string>
retrieve_accessors(const std::string &value) {
   std::string accessor;
   std::vector<std::string> accessor_list;
   std::stringstream source(value);
   while (std::getline(source, accessor, '.')) {
      accessor_list.push_back(accessor);
   }
   return accessor_list;
}

namespace sauros {

bool environment_c::exists(const std::string &item) {
#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("environment_c::exists");
#endif
   if (_env.find(item) != _env.end()) {
      return true;
   }

   if (_parent) {
      return _parent->exists(item);
   }

   return false;
}

environment_c::environment_c(cells_t &params, cells_t &args,
                             std::shared_ptr<environment_c> outer)
    : _parent(outer) {
#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("environment_c::environment_c");
#endif

   auto arg = args.begin();
   for (auto param = params.begin(); param != params.end(); ++param) {
      _env[(*param)->data] = *arg++;
   }
}

void environment_c::set(const std::string &item, cell_ptr cell) {
#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("environment_c::set");
#endif
   _env[item] = cell;
}

cell_ptr &environment_c::get(const std::string &item) {
#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("environment_c::get");
#endif
   return _env[item];
}

environment_c *environment_c::find(const std::string &var,
                                   cell_ptr origin_cell) {
#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("environment_c::find");
#endif

   if (_env.find(var) != _env.end()) {
      return this;
   }
   if (_parent) {
      return _parent->find(var, origin_cell);
   }

   throw unknown_identifier_c(var, origin_cell);
}

bool environment_c::package_loaded(const std::string &package) {
#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("environment_c::package_loaded");
#endif
   if (_loaded_packages.find(package) != _loaded_packages.end()) {
      return true;
   }
   if (_parent) {
      return _parent->package_loaded(package);
   }
   return false;
}

void environment_c::save_package(const std::string &name,
                                 std::shared_ptr<rll_wrapper_c> lib) {
#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("environment_c::save_package");
#endif
   _loaded_packages[name] = lib;
}

} // namespace sauros