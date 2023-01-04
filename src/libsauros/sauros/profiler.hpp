/*
   This is a simple profile utility used to determine
   how many time a given function/ instruction is executed
   for a given sau file.

   Typically singletons provide a source of bad code smell,
   but the profiler is meant to be dropped in anywhere that
   someone may be called whenever the option is enabled in
   the build system... no better way
*/
#ifdef PROFILER_ENABLED

#ifndef SAUROS_PROFILER_HPP
#define SAUROS_PROFILER_HPP

#include "rang.hpp"
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>

namespace sauros {

class profiler_c {

 public:
   profiler_c(profiler_c &) = delete;
   void operator=(const profiler_c &) = delete;

   static profiler_c *get_profiler() {
      if (!_profiler) {
         _profiler = new profiler_c();
      }
      return _profiler;
   }

   void hit(const std::string source) {
      if (_hits.find(source) != _hits.end()) {
         _hits[source]++;
         return;
      }
      _hits[source] = 1;
   }

   void dump() {
      std::cout << rang::fg::yellow << "Profile dump\n"
                << rang::fg::reset
                << "The following items may not show in their execution order\n"
                << std::endl;
      for (auto [source, count] : _hits) {
         std::cout << rang::fg::cyan << source << rang::fg::reset << " : "
                   << count << std::endl;
      }
   }

 protected:
   static profiler_c *_profiler;

   profiler_c() {}

   std::unordered_map<std::string, uint64_t> _hits;
};

} // namespace sauros

#endif
#endif