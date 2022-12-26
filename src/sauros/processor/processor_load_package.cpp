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

      // std::cout << "already loaded\n";
      return;
   }

   auto sauros_home = _system.get_sauros_directory();

   if (!sauros_home.has_value()) {
      throw runtime_exception_c("sauros home directory not found. please set "
                                "SAUROS_HOME environment variable.",
                                location);
   }

   std::filesystem::path target_manifest_file = (*sauros_home);
   target_manifest_file /= "packages";
   target_manifest_file /= target;

   auto submodule_root = target_manifest_file;

   target_manifest_file /= "package.sau";

   std::cout << "looking for package : " << target_manifest_file.c_str() <<
   std::endl;

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

   struct submodule_s {
      std::string name;
      std::string library_file;
      std::vector<std::string> library_function_list;
      std::vector<std::string> source_file_list;
      std::shared_ptr<sauros::environment_c> env{nullptr};
   };
   std::vector<submodule_s> submodules_list;

   std::string package_name;
   std::vector<std::string> package_source_files;

   {
      if (!package_load_env->exists("package_name")) {
         throw runtime_exception_c("package name for target: " + target +
                                       " does not contain a target_name",
                                   location);
      }

      auto package_name_cell = package_load_env->get("package_name");
      if (package_name_cell->type != cell_type_e::STRING) {
         throw runtime_exception_c("package_name in package.sau for " + target +
                                       " is not of type `STRING`",
                                   location);
      }
      package_name = package_name_cell->data;

      // Lambda to load a submodule
      auto load_submodule_fn = [=](const std::string& name) {

         std::cout << "load submodule: " << name << std::endl; 

         auto submodule_file = submodule_root;
         submodule_file /= name;
         submodule_file /= "submodule.sau";

         submodule_s submodule;
         submodule.env = std::make_shared<sauros::environment_c>();

         {
            sauros::file_executor_c file_executor(submodule.env);
            if (0 != file_executor.run(submodule_file.c_str())) {
               throw runtime_exception_c("unable to open file " +
                                             std::string(submodule_file),
                                       location);
            }
         }

         // Get the name
         //
         if (!submodule.env->exists("submodule_name")) {
            throw runtime_exception_c("submodule: " + name +
                                          " does not contain a submodule_name",
                                    location);
         }
         auto submodule_name_cell = submodule.env->get("submodule_name");
         if (submodule_name_cell->type != cell_type_e::STRING) {
            throw runtime_exception_c("submodule_name in submodule.sau for " + name +
                                          " is not of type `STRING`",
                                    location);
         }
         submodule.name = submodule_name_cell->data;


         std::cout << ">>>>>>> submodule_name: " << submodule.name << std::endl;

         // Check if we need to load library info
         if (submodule.env->exists("library_file")) {
            
            auto library_file_cell = submodule.env->get("library_file");
            if (library_file_cell->type != cell_type_e::STRING) {
               throw runtime_exception_c("library_file in submodule.sau for " + name +
                                             " is not of type `STRING`",
                                       location);
            }
            submodule.library_file = library_file_cell->data;

            std::cout << "\t[library file] " << submodule.library_file << std::endl;

            if (!submodule.env->exists("library_functions")) {
               throw runtime_exception_c("submodule: " + name +
                                             " does not contain a library_functions list for library listed as: " +  submodule.library_file,
                                       location);
            }

            auto library_functions_cell = submodule.env->get("library_functions");
            if (library_functions_cell->type != cell_type_e::LIST) {
               throw runtime_exception_c("library_functions in submodule.sau for " + name +
                                             " is not of type `LIST`",
                                       location);
            }

            // Load teh functions 
            for (auto& function_name_cell : library_functions_cell->list) {
               if (function_name_cell->type != cell_type_e::STRING) {
                  throw runtime_exception_c("function name listed in submodule.sau for " + name +
                                                " is not of type `STRING`",
                                          location);
               }
               submodule.library_function_list.push_back(function_name_cell->data);

               std::cout << "\t[library function] " << function_name_cell->data << std::endl;
            }
         }

         // Check if there are source files listed
         
         if (submodule.env->exists("source_files")) {
            auto source_files_cell = submodule.env->get("source_files");
            if (source_files_cell->type != cell_type_e::LIST) {
               throw runtime_exception_c("source_files in submodule.sau for " + name +
                                             " is not of type `LIST`",
                                       location);
            }

            // Load teh functions 
            for (auto& file_name_cell : source_files_cell->list) {
               if (file_name_cell->type != cell_type_e::STRING) {
                  throw runtime_exception_c("file name listed in submodule.sau for " + name +
                                                " is not of type `STRING`",
                                          location);
               }
               submodule.source_file_list.push_back(file_name_cell->data);

               std::cout << "\t[source file] " << file_name_cell->data << std::endl;
            }

         }

      }; // End submodule load lambda


      
      // Check if submodules exist
      if (package_load_env->exists("submodules")) {
         auto submodule_list_cell = package_load_env->get("submodules");
         if (submodule_list_cell->type != cell_type_e::LIST) {
            throw runtime_exception_c("submodules in package.sau for " + target +
                                          " is not of type `LIST`",
                                    location);
         }
         for(auto& submodule_cell : submodule_list_cell->list) {
            if (submodule_cell->type != cell_type_e::STRING) {
               throw runtime_exception_c("submodule listed in submodules list in package.sau for " + target +
                                             " is not of type `STRING`",
                                       location);
            }
            load_submodule_fn(submodule_cell->data);
         }
      }

      if (package_load_env->exists("source_files")) {

         std::cout << "package sources" << std::endl;

         auto source_files_cell = package_load_env->get("source_files");
         if (source_files_cell->type != cell_type_e::LIST) {
            throw runtime_exception_c("source_files in package.sau is not of type `LIST`",
                                    location);
         }

         // Load teh functions 
         for (auto& file_name_cell : source_files_cell->list) {
            if (file_name_cell->type != cell_type_e::STRING) {
               throw runtime_exception_c("file name listed in package.sau's source_files is not of type `STRING`",
                                       location);
            }
            package_source_files.push_back(file_name_cell->data);

            std::cout << "\t[source file] " << file_name_cell->data << std::endl;
         }
      }
   }
/*
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
      env->set(scoped_name, std::make_shared<cell_c>(fn));
   }

   */
}

} // namespace sauros