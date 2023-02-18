#ifndef SAUROS_PACKAGE_HPP
#define SAUROS_PACKAGE_HPP

#include "sauros/environment.hpp"
#include "sauros/system/system.hpp"
#include <memory>
#include <string>
#include <vector>

namespace sauros {
namespace package {
struct pkg_s {
   std::string name;
   std::string library_file;
   std::string license;
   std::string version;
   std::vector<std::string> authors_list;
   std::vector<std::string> library_function_list;
   std::vector<std::string> source_file_list;
   std::vector<std::string> requires_list;
   std::shared_ptr<sauros::environment_c> env{nullptr};
};

//! \brief Attempt to load a package from a target directory
extern pkg_s load(cell_ptr cell, sauros::system_c &system, location_s *location,
                  env_ptr env);
} // namespace package
} // namespace sauros
#endif