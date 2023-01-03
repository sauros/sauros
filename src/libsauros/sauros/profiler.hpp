
#ifdef PROFILER_ENABLED

#ifndef SAUROS_PROFILER_HPP
#define SAUROS_PROFILER_HPP

#include <string>
#include <unordered_map>
#include <cstdint>
#include <iostream>
#include "rang.hpp"
#include <iomanip>

namespace sauros {

class profiler_c {

public:
   profiler_c(profiler_c&) = delete;
   void operator= (const profiler_c&) = delete;

   static profiler_c* get_profiler(){
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
      for(auto [source, count] : _hits) {
         std::cout << rang::fg::cyan << std::setw(20) << source << rang::fg::reset << ": " << count << std::endl;
      }
   }

protected:
   static profiler_c * _profiler;

   profiler_c(){}

   std::unordered_map<std::string, uint64_t> _hits;
};


}

#endif
#endif