#ifndef SAUROS_SYSTEM_HOME_HPP
#define SAUROS_SYSTEM_HOME_HPP

#include "profiler.hpp"
#include <filesystem>
#include <optional>
#include <string>

namespace sauros {
namespace system {

static inline std::optional<std::string> sauros_home() {

#ifdef PROFILER_ENABLED
  profiler_c::get_profiler()->hit("system::sauros_home");
#endif

  std::filesystem::path sauros_home;

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
    return {};
#endif

    if (!std::filesystem::exists(user_home)) {
      return {};
    }

    sauros_home = user_home / std::filesystem::path(".sauros");
  }

  if (std::filesystem::exists(sauros_home)) {
    return {sauros_home.string()};
  }
  return {};
}
} // namespace system
} // namespace sauros

#endif