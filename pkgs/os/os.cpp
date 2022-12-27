#include "io.hpp"

#include <iostream>
#include <string>
#include <filesystem>

sauros::cell_ptr
_pkg_os_cwd_(sauros::cells_t &cells,
                     std::shared_ptr<sauros::environment_c> env) {
   return std::make_shared<sauros::cell_c>(
      sauros::cell_type_e::STRING, std::filesystem::current_path().string());
}
