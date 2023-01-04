#include "system.hpp"
#include <cstdlib>

#include "sauros/profiler.hpp"
#include <filesystem>
#include <iostream>

namespace sauros {

system_c::system_c() {

#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("system_c::system_c");
#endif

   std::filesystem::path sauros_home;

   // First check to see if there is a SAUROS_HOME that will override defaults

   if (const char *s_home = std::getenv("SAUROS_HOME")) {

      sauros_home = std::filesystem::path(s_home);
   } else {

      std::filesystem::path user_home;

#if defined(__linux__) || defined(__APPLE__) || defined(__unix__)
      if (const char *unix_home = std::getenv("HOME")) {
         user_home = std::filesystem::path(unix_home);
      }

#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
      if (const char *home_drive = std::getenv("HOMEDRIVE")) {
         if (const char *home_path = std::getenv("HOMEPATH")) {
            user_home = std::filesystem::path(home_drive);
            std::filesystem::path path(home_path);
            user_home /= path;
         }
      }
#else
      // Unable to detect the home env
#endif

      if (!std::filesystem::exists(user_home)) {
         // std::cerr << "Suspected home directory `" << user_home.c_str() << "`
         // does not exist - Unable to determine sauors home" << std::endl;
         return;
      }

      sauros_home = user_home / std::filesystem::path(".sauros");
   }

   if (std::filesystem::exists(sauros_home)) {
      _home = sauros_home.c_str();
      // std::cout << "sauros home exists" << std::endl;
      return;
   }
}

} // namespace sauros