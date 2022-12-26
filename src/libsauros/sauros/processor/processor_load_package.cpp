#include "../driver.hpp"
#include "processor.hpp"
#include <filesystem>
#include <iostream>

#include "../RLL/RLL.h"

namespace sauros {

// The RLL object blows up if its in a header, likely because
// of the use of .inl files for platform specifics. Because of
// that we forward declare the type in the header for the processor
// then we operate wih it here.

// Investigating the issue is backlogged.

processor_c::~processor_c() {
   for (auto [key, value] : _loaded_package) {
      if (value) {
         delete value;
      }
   }
}

void processor_c::load_package(const std::string &target, location_s location,
                               std::shared_ptr<environment_c> env) {

   // Check to see if its already loaded
   if (_loaded_package.find(target) != _loaded_package.end()) {
      return;
   }

   auto sauros_home = _system.get_sauros_directory();

   if (!sauros_home.has_value()) {
      throw runtime_exception_c("sauros home directory not found. please set "
                                "SAUROS_HOME environment variable.",
                                location);
   }

   std::filesystem::path target_manifest_file = (*sauros_home);
   target_manifest_file /= "pkgs";
   target_manifest_file /= target;

   auto root = target_manifest_file;

   target_manifest_file /= "pkg.sau";

   //std::cout << "looking for package : " << target_manifest_file.c_str()
            // << std::endl;

   if (!std::filesystem::is_regular_file(target_manifest_file)) {
      throw runtime_exception_c(
          "unable to locate package file for given package: " + target,
          location);
   }

   // Now we load the manifest in a new processor and environment
   std::shared_ptr<sauros::environment_c> package_load_env =
       std::make_shared<sauros::environment_c>();

   {
      sauros::file_executor_c file_executor(package_load_env);
      if (0 != file_executor.run(target_manifest_file.c_str())) {
         throw runtime_exception_c("unable to open file " +
                                       std::string(target_manifest_file),
                                   location);
      }
   }

   // Now the environment has the information we need to
   // load the environment given to us with the library data

   struct pkg_s {
      std::string name;
      std::string library_file;
      std::vector<std::string> library_function_list;
      std::vector<std::string> source_file_list;
      std::shared_ptr<sauros::environment_c> env{nullptr};
   };

   pkg_s package;

   {
      if (!package_load_env->exists("pkg_name")) {
         throw runtime_exception_c(
             " target: " + target + " does not contain a pkg_name", location);
      }

      auto package_name_cell = package_load_env->get("pkg_name");
      if (package_name_cell->type != cell_type_e::STRING) {
         throw runtime_exception_c("pkg_name in pkg.sau for " + target +
                                       " is not of type `STRING`",
                                   location);
      }
      package.name = package_name_cell->data;

      auto package_file = root;
      package_file /= "pkg.sau";

      package.env = std::make_shared<sauros::environment_c>();

      {
         sauros::file_executor_c file_executor(package.env);
         if (0 != file_executor.run(package_file.c_str())) {
            throw runtime_exception_c(
                "unable to open file " + std::string(package_file), location);
         }
      }

      //std::cout << "pkg_name: " << package.name << std::endl;

      // Check if we need to load library info
      if (package.env->exists("library_file")) {

         auto library_file_cell = package.env->get("library_file");
         if (library_file_cell->type != cell_type_e::STRING) {
            throw runtime_exception_c("library_file in package.sau for " +
                                          target + " is not of type `STRING`",
                                      location);
         }

         //std::cout << "\t[library file] " << library_file_cell->data
                  // << std::endl;

         {
            auto library_file_actual = root;
            library_file_actual /= library_file_cell->data;
            if (!std::filesystem::is_regular_file(library_file_actual)) {
               throw runtime_exception_c(
                   "library file:" + library_file_actual.string() +
                       " for package: " + target + " does not exist",
                   location);
            }
            package.library_file = library_file_actual.string();
         }

         if (!package.env->exists("library_functions")) {
            throw runtime_exception_c(
                target +
                    " does not contain a library_functions list for library "
                    "listed as: " +
                    package.library_file,
                location);
         }

         auto library_functions_cell = package.env->get("library_functions");
         if (library_functions_cell->type != cell_type_e::LIST) {
            throw runtime_exception_c("library_functions in package.sau for " +
                                          target + " is not of type `LIST`",
                                      location);
         }

         // Load teh functions
         for (auto &function_name_cell : library_functions_cell->list) {
            if (function_name_cell->type != cell_type_e::STRING) {
               throw runtime_exception_c(
                   "function name listed in package.sau for " + target +
                       " is not of type `STRING`",
                   location);
            }
            package.library_function_list.push_back(function_name_cell->data);

            //std::cout << "\t[library function] " << function_name_cell->data
                    //  << std::endl;
         }
      }

      // Check if there are source files listed

      if (package.env->exists("source_files")) {
         auto source_files_cell = package.env->get("source_files");
         if (source_files_cell->type != cell_type_e::LIST) {
            throw runtime_exception_c("source_files in package.sau for " +
                                          target + " is not of type `LIST`",
                                      location);
         }

         // Load teh functions
         for (auto &file_name_cell : source_files_cell->list) {
            if (file_name_cell->type != cell_type_e::STRING) {
               throw runtime_exception_c(
                   "file name listed in package.sau for " + target +
                       " is not of type `STRING`",
                   location);
            }

            {
               auto file_actual = root;
               file_actual /= file_name_cell->data;
               if (!std::filesystem::is_regular_file(file_actual)) {
                  throw runtime_exception_c(
                      "source file:" + file_actual.string() +
                          " for package: " + package.name + " does not exist",
                      location);
               }
               package.source_file_list.push_back(file_actual);
            }

            //std::cout << "\t[source file] " << file_name_cell->data
                   //   << std::endl;
         }
      }

   } // End anon scope

   // Make the cell that will encompass all imports
   auto boxed_cell = std::make_shared<cell_c>(cell_type_e::BOX);
   boxed_cell->box_env = std::make_shared<environment_c>();

   //
   //    Load the library
   //

   auto lib = new rll::shared_library();

   try {
      lib->load(package.library_file.c_str());
   } catch (rll::exception::library_loading_error &e) {
      delete lib;
      throw runtime_exception_c("error loading `" + target + "`: " + e.what(),
                                location);
   }

   if (!lib->is_loaded()) {
      delete lib;
      throw runtime_exception_c(
          "failed to load library: `" + package.library_file + "`", location);
   }

   //std::cout << "Library loaded\n";

   for (auto &f : package.library_function_list) {

      //std::cout << "Loading function : " << f << std::endl;

      if (!lib->has_symbol(f)) {
         throw runtime_exception_c(
             "error loading `" + target + "`: listed function `" + f +
                 "` was not found in `" + package.library_file.c_str() + "`",
             location);
      }

      void *fn_ptr = lib->get_symbol(f);
      cell_c::proc_f fn = reinterpret_cast<cell_ptr (*)(
          cells_t &, std::shared_ptr<environment_c>)>(fn_ptr);

      // Add to env
      boxed_cell->box_env->set(f, std::make_shared<cell_c>(fn));
   }

   //
   //    Load the source files
   //

   for (auto &f : package.source_file_list) {

      //std::cout << "Loading source file : " << f << std::endl;

      sauros::file_executor_c file_executor(boxed_cell->box_env);
      if (0 != file_executor.run(f)) {
         throw runtime_exception_c("unable to open file " + std::string(f),
                                   location);
      }
   }

   //
   //    Add the library to the main enviornment
   //

   env->set(package.name, boxed_cell);
}

} // namespace sauros