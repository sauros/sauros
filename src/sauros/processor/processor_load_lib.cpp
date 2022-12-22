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
   for (auto [key, value] : _loaded_libs) {
      if (value) {
         delete value;
      }
   }
}

void processor_c::load_library(const std::string &target, location_s location,
                               std::shared_ptr<environment_c> env) {

   // Check to see if its already loaded
   if (_loaded_libs.find(target) != _loaded_libs.end()) {

      // std::cout << "already loaded\n";
      return;
   }

   // std::cout << "Attempting to load: " << target << std::endl;

   // Check for /home/user/.sauros/libs/<target>/manifest.sau

   auto sauros_home = _system.get_sauros_directory();

   if (!sauros_home.has_value()) {
      throw runtime_exception_c("sauros home directory not found. please set "
                                "SAUROS_HOME environment variable.",
                                location);
   }

   std::filesystem::path target_manifest_file = (*sauros_home);
   target_manifest_file /= "libs";
   target_manifest_file /= target;
   target_manifest_file /= "manifest.sau";

   // std::cout << "looking for : " << target_manifest_file.c_str() <<
   // std::endl;

   if (!std::filesystem::is_regular_file(target_manifest_file)) {
      throw runtime_exception_c(
          "unable to locate manifest file for given library: " + target,
          location);
   }

   // Now we load the manifest in a new processor and environment
   std::shared_ptr<sauros::environment_c> manifest_env =
       std::make_shared<sauros::environment_c>();

   {
      sauros::file_executor_c file_executor(manifest_env);
      if (0 != file_executor.run(target_manifest_file.c_str())) {
         throw runtime_exception_c("unable to open file " + std::string(target_manifest_file),
                                   location);
      }
   }

   // Now the environment has the information we need to
   // load the environment given to us with the library data

   std::string target_name;
   std::string prefix;
   std::vector<std::string> function_list;

   {
      if (!manifest_env->exists("target_name")) {
         throw runtime_exception_c("manifest for target: " + target +
                                       " does not contain a target_name",
                                   location);
      }

      auto target_name_cell = manifest_env->get("target_name");
      if (target_name_cell->type != cell_type_e::STRING) {
         throw runtime_exception_c("target_name in manifest for " + target +
                                       " is not of type `STRING`",
                                   location);
      }
      target_name = target_name_cell->data;

      if (!manifest_env->exists("prefix")) {
         throw runtime_exception_c("manifest for target: " + target +
                                       " does not contain a prefix",
                                   location);
      }

      auto prefix_cell = manifest_env->get("prefix");
      if (prefix_cell->type != cell_type_e::STRING) {
         throw runtime_exception_c("prefix in manifest for " + target +
                                       " is not of type `STRING`",
                                   location);
      }
      prefix = prefix_cell->data;

      if (prefix.empty()) {
         throw runtime_exception_c("prefix in manifest for " + target +
                                       " can not be empty",
                                   location);
      }

      if (!manifest_env->exists("function_list")) {
         throw runtime_exception_c("manifest for target: " + target +
                                       " does not contain a function_list",
                                   location);
      }

      auto function_list_cell = manifest_env->get("function_list");
      if (function_list_cell->type != cell_type_e::LIST) {
         throw runtime_exception_c("function_list in manifest for " + target +
                                       " is not of type `LIST`",
                                   location);
      }

      for (auto el : function_list_cell->list) {
         if (el->type != cell_type_e::STRING) {
            throw runtime_exception_c(
                "item in " + target + " function_list is not of type `STRING`",
                location);
         }
         function_list.push_back(el->data);
      }
   }

   std::filesystem::path target_manifest_library = (*sauros_home);
   target_manifest_library /= "libs";
   target_manifest_library /= target;
   target_manifest_library /= target_name;

   if (!std::filesystem::is_regular_file(target_manifest_library)) {
      throw runtime_exception_c(target_manifest_library.string() +
                                    " does not exist within target " + target,
                                location);
   }

   auto lib = new rll::shared_library();

   try {
      lib->load(target_manifest_library.c_str());
   } catch (rll::exception::library_loading_error &e) {
      delete lib;
      throw runtime_exception_c("error loading `" + target + "`: " + e.what(),
                                location);
   }

   if (!lib->is_loaded()) {
      delete lib;
      throw runtime_exception_c("failed to load library: `" +
                                    target_manifest_library.string() + "`",
                                location);
   }

   // std::cout << "Library loaded\n";

   for (auto &f : function_list) {

      // std::cout << "Loading function : " << f << std::endl;

      if (!lib->has_symbol(f)) {
         throw runtime_exception_c(
             "error loading `" + target + "`: listed function `" + f +
                 "` was not found in `" + target_manifest_library.c_str() + "`",
             location);
      }

      void *fn_ptr = lib->get_symbol(f);
      cell_c::proc_f fn = reinterpret_cast<cell_ptr (*)(
          cells_t &, std::shared_ptr<environment_c>)>(fn_ptr);

      // Add to env
      std::string scoped_name = prefix + "::" + f;
      env->set(scoped_name,  std::make_shared<cell_c>(fn));
   }
}

} // namespace sauros