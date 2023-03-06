#include "package.hpp"
#include <sauros/rang.hpp>

#include <iostream>

namespace mgmt {

std::optional<sauros::package::pkg_s>
package_load_from_dir(std::filesystem::path dir) {

  sauros::package::pkg_s result;

  if (!dir.is_absolute()) {
    dir = std::filesystem::absolute(dir);
  }

  if (!std::filesystem::is_directory(dir)) {
    std::cerr << rang::fg::red << "[FAIL]" << rang::fg::reset
              << " Given item is not a directory" << std::endl;
    return {};
  }

  auto dino_file = dir;
  dir /= "dino.saur";

  if (!std::filesystem::is_regular_file(dino_file)) {
    std::cerr << rang::fg::red << "[FAIL]" << rang::fg::reset
              << " Target directory does not contain a top-level dino.saur"
              << std::endl;
    return {};
  }

  return {result};
}

} // namespace mgmt