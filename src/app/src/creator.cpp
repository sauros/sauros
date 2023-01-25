#include "creator.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sauros/format.hpp>

namespace app {

namespace package {

std::string header =
    "#ifndef SAUROS_PKG_HPP\n"
    "#define SAUROS_PKG_HPP\n"
    "#include <sauros/sauros.hpp>\n"
    "#ifdef WIN32\n"
    "#define API_EXPORT __declspec(dllexport)\n"
    "#else\n"
    "#define API_EXPORT\n"
    "#endif\n"
    "extern \"C\" {\n"
    "/*\n"
    "   A callable function!\n"
    "*/\n"
    "API_EXPORT\n"
    "extern sauros::cell_ptr\n"
    "_pkg_callable_(sauros::cells_t &cells,\n"
    "                        std::shared_ptr<sauros::environment_c> env);\n"
    "}\n"
    "#endif\n";

std::string source =
    "#include \"package.hpp\"\n"
    "#include <sauros/capi/capi.hpp>\n"
    "#include <string>\n"
    "\n"
    "sauros::cell_ptr _pkg_callable_(sauros::cells_t &cells,\n"
    "                                  std::shared_ptr<sauros::environment_c> "
    "env) {\n"
    "   return std::make_shared<sauros::cell_c>(sauros::cell_type_e::INTEGER, "
    "(sauros::cell_int_t)1);\n"
    "}\n";

std::string generate_cmake_lists(std::string package_name) {

   return sauros::format("cmake_minimum_required(VERSION 3.5)\n"
                         "project (%)\n"
                         "\n"
                         "set(CMAKE_CXX_STANDARD 20)\n"
                         "\n"
                         "add_library(%_lib SHARED package.cpp)\n"
                         "set_target_properties(%_lib PROPERTIES PREFIX \"\" "
                         "SUFFIX \".lib\" OUTPUT_NAME \"%\")\n",
                         package_name, package_name, package_name,
                         package_name);
}

std::string generate_pkg_saur_cpp(std::string package_name) {
   return sauros::format("[var pkg_name \"%\"]\n"
                         "\n"
                         "; Optional auhor/license fields\n"
                         ";\n"
                         ";[var authors \"<AUTHOR>\"]\n"
                         ";[var license \"<LICENSE>\"]\n"
                         "\n"
                         "; C++ shared lib to link to\n"
                         "[var library_file \"%.lib\"]\n"
                         "[var library_functions [list\n"
                         "   \"_pkg_callable_\"\n"
                         "]]\n"
                         "\n"
                         "; Optional requirements for package\n"
                         "; that will trigger the search for\n"
                         "; required packages upon loading\n"
                         ";\n"
                         "; [var requires [list \"std\"]]\n"
                         "\n"
                         "; Files to include under the current pkg\n"
                         "[var source_files [list\n"
                         "   \"%.saur\"\n"
                         "]]\n",
                         package_name, package_name, package_name);
}

std::string generate_pkg_saur(std::string package_name) {
   return sauros::format("[var pkg_name \"%\"]\n"
                         "\n"
                         "; Optional auhor/license fields\n"
                         ";\n"
                         ";[var authors \"<AUTHOR>\"]\n"
                         ";[var license \"<LICENSE>\"]\n"
                         "\n"
                         "; Optional requirements for package\n"
                         "; that will trigger the search for\n"
                         "; required packages upon loading\n"
                         ";\n"
                         "; [var requires [list \"std\"]]\n"
                         "\n"
                         "; Files to include under the current pkg\n"
                         "[var source_files [list\n"
                         "   \"%.saur\"\n"
                         "]]\n",
                         package_name, package_name);
}

std::string generate_named_saur(std::string package_name) {

   return sauros::format(
       "[var callable [lambda [] [putln \"hello from \" \"%\"] ]]\n",
       package_name);
}

std::string generate_named_saur_cpp(std::string package_name) {

   return sauros::format(
       "; While not required, it is common practice to name callable c++ "
       "methods\n"
       "; with a leading __ indicating not to call them directly, and instead "
       "we wrap\n"
       "; them in a lambda. This is to ensure that the parameters required for "
       "the underlying\n"
       "; callable function are checked by sauros upon calling the lambda\n"
       "\n"
       "; Also note how we prefix the callable with (%) as lambdas capture the "
       "environment\n"
       "; that they are called from, not defined in. Since we know (%) will be "
       "how the caller\n"
       "; accesses the package, we can safely prefix the c++ call"
       "\n"
       "[var callable [lambda [] [%._pkg_callable_]]]\n",
       package_name, package_name, package_name);
}

} // namespace package

bool write(std::string file, std::string contents) {
   std::ofstream s;
   s.open(file);
   if (!s.is_open()) {
      return false;
   }
   s << contents;
   s.close();
   return true;
}

int create_package(std::string &file, bool with_cpp) {

   std::cout << "Create package: " << file << std::endl;

   auto path = std::filesystem::path(file);

   if (std::filesystem::exists(path)) {
      std::cerr << "Given path: " << file
                << " already exists. Can not create package" << std::endl;
      return 1;
   }

   auto package_name = path.filename().string();

   std::cout << "name: " << package_name << std::endl;
   ;

   if (!std::filesystem::create_directory(path)) {
      std::cerr << "Unable to create directory: " << file << std::endl;
      return 1;
   }

   if (with_cpp) {
      auto current_file = path / "CMakeLists.txt";
      if (!write(current_file.c_str(),
                 package::generate_cmake_lists(package_name))) {
         std::cerr << "Unable to write file: " << current_file << std::endl;
         return 1;
      }
      {
         auto current_file = path / "package.hpp";
         if (!write(current_file.c_str(), package::header)) {
            std::cerr << "Unable to write file: " << current_file << std::endl;
            return 1;
         }
      }
      {
         auto current_file = path / "package.cpp";
         if (!write(current_file.c_str(), package::source)) {
            std::cerr << "Unable to write file: " << current_file << std::endl;
            return 1;
         }
      }
      {
         auto current_file = path / "pkg.saur";
         if (!write(current_file.c_str(),
                    package::generate_pkg_saur_cpp(package_name))) {
            std::cerr << "Unable to write file: " << current_file << std::endl;
            return 1;
         }
      }
      {
         auto current_file = path / (package_name + ".saur");
         if (!write(current_file.c_str(),
                    package::generate_named_saur_cpp(package_name))) {
            std::cerr << "Unable to write file: " << current_file << std::endl;
            return 1;
         }
      }
   } else {

      {
         auto current_file = path / "pkg.saur";
         if (!write(current_file.c_str(),
                    package::generate_pkg_saur(package_name))) {
            std::cerr << "Unable to write file: " << current_file << std::endl;
            return 1;
         }
      }
      {
         auto current_file = path / (package_name + ".saur");
         if (!write(current_file.c_str(),
                    package::generate_named_saur(package_name))) {
            std::cerr << "Unable to write file: " << current_file << std::endl;
            return 1;
         }
      }
   }

   return 1;
}

} // namespace app