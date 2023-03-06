#ifndef SAUROS_APP_PACKAGE_HPP
#define SAUROS_APP_PACKAGE_HPP

#include <filesystem>
#include <optional>
#include <sauros/package/package.hpp>
#include <string>
#include <vector>

namespace mgmt {

extern std::optional<sauros::package::pkg_s>
package_load_from_dir(std::filesystem::path dir);

}

#endif