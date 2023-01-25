#include "package.hpp"

#include "sauros/driver.hpp"
#include "sauros/format.hpp"
#include "sauros/processor/processor.hpp"
#include "sauros/profiler.hpp"

#include <filesystem>
#include <iostream>

namespace sauros {
namespace package {

#define PACKAGE_CHECK(condition__, msg__)                                      \
   if (!(condition__)) {                                                       \
      throw sauros::processor_c::runtime_exception_c(                          \
          msg__, std::make_shared<cell_c>(cell_type_e::STRING, "", location)); \
   }

extern pkg_s load(cell_ptr cell, sauros::system_c &system, location_s *location,
                  std::shared_ptr<environment_c> env) {

#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("package::load");
#endif

   auto sauros_home = system.get_sauros_directory();

   PACKAGE_CHECK(sauros_home.has_value(),
                 "sauros home directory not found. please set "
                 "SAUROS_HOME environment variable.")

   auto target = cell->data_as_str();

   std::filesystem::path root;
   std::filesystem::path target_manifest_file;

   // Prefer the local package to an installed one
   if (cell->origin) {
      target_manifest_file = (*cell->origin);
      target_manifest_file.remove_filename();
      target_manifest_file /= target;
      root = target_manifest_file;
      target_manifest_file /= "pkg.saur";
   }

   // If a local one doesn't exist then we target install location
   if (!std::filesystem::is_regular_file(target_manifest_file)) {
      target_manifest_file = (*cell->origin);
      target_manifest_file /= "pkgs";
      target_manifest_file /= target;
      root = target_manifest_file;
      target_manifest_file /= "pkg.saur";
   }

   // std::cout << "looking for package : " << target_manifest_file.c_str()
   //  << std::endl;

   PACKAGE_CHECK(std::filesystem::is_regular_file(target_manifest_file),
                 sauros::format("unable to locate package for: %", target))

   pkg_s package;

   // Now we load the manifest in a new processor and environment
   package.env = std::make_shared<sauros::environment_c>();

   {
      sauros::file_executor_c file_executor(package.env);
      PACKAGE_CHECK(!file_executor.run(target_manifest_file.c_str()),
                    sauros::format("unable to open: %",
                                   std::string(target_manifest_file)))
   }

   // Now the environment has the information we need to
   // load the environment given to us with the library data

   {
      PACKAGE_CHECK(
          package.env->exists("pkg_name"),
          sauros::format(
              "target: % does not contain the required field `pkg_name`",
              target))

      // Optional
      if (package.env->exists("authors")) {
         auto author_cell = package.env->get("authors");
         if (author_cell->type == cell_type_e::STRING) {
            package.authors_list.push_back(author_cell->data_as_str());
         } else {
            PACKAGE_CHECK(
                (author_cell->type == cell_type_e::LIST),
                "author field in pkg must be of type `string` or type `list`")

            for (auto &author : author_cell->list) {
               PACKAGE_CHECK(
                   (author->type == cell_type_e::STRING),
                   "author name in pkg authors list must be of type `string`")
               package.authors_list.push_back(author->data_as_str());
            }
         }
      }

      // Optional
      if (package.env->exists("license")) {
         auto license_cell = package.env->get("license");

         PACKAGE_CHECK((license_cell->type == cell_type_e::STRING),
                       "license field in pkg must be of type `string`")

         package.license = license_cell->data_as_str();
      }

      auto package_name_cell = package.env->get("pkg_name");

      PACKAGE_CHECK((package_name_cell->type == cell_type_e::STRING),
                    "pkg_name field in pkg must be of type `string`")

      package.name = package_name_cell->data_as_str();

      auto package_file = root;
      package_file /= "pkg.saur";

      // Optional
      if (package.env->exists("requires")) {
         auto requires_cell = package.env->get("requires");
         PACKAGE_CHECK((requires_cell->type == cell_type_e::LIST),
                       "requires field in pkg must be of type `string`")
         for (auto &required_pkg : requires_cell->list) {
            PACKAGE_CHECK(
                (required_pkg->type == cell_type_e::STRING),
                "package name in pkg's requires list must be of type `string`")
            package.requires_list.push_back(required_pkg->data_as_str());
         }
      }

      // Check if we need to load library info
      if (package.env->exists("library_file")) {

         auto library_file_cell = package.env->get("library_file");

         PACKAGE_CHECK((package_name_cell->type == cell_type_e::STRING),
                       "library_file field in pkg must be of type `string`")

         // std::cout << "\t[library file] " << library_file_cell->data
         //  << std::endl;

         {
            auto library_file_actual = root;
            library_file_actual /= *library_file_cell->data.s;

            PACKAGE_CHECK(
                (std::filesystem::is_regular_file(library_file_actual)),
                sauros::format(
                    "given library_file target: %  for pkg % does not exist",
                    library_file_actual.string(), target))

            package.library_file = library_file_actual.string();
         }

         PACKAGE_CHECK(
             package.env->exists("library_functions"),
             sauros::format(
                 "% does not contain a library_functions list for library: %",
                 target, package.library_file))

         auto library_functions_cell = package.env->get("library_functions");

         PACKAGE_CHECK(
             (library_functions_cell->type == cell_type_e::LIST),
             "library_functions_cell field in pkg must be of type `list`")

         // Load teh functions
         for (auto &function_name_cell : library_functions_cell->list) {

            PACKAGE_CHECK((function_name_cell->type == cell_type_e::STRING),
                          "function name in pkg's library_functions list must "
                          "be of type `string`")

            package.library_function_list.push_back(
                function_name_cell->data_as_str());

            // std::cout << "\t[library function] " << function_name_cell->data
            //   << std::endl;
         }
      }

      // Check if there are source files listed

      if (package.env->exists("source_files")) {
         auto source_files_cell = package.env->get("source_files");
         PACKAGE_CHECK((source_files_cell->type == cell_type_e::LIST),
                       "source_files field in pkg must be of type `list`")

         // Load teh functions
         for (auto &file_name_cell : source_files_cell->list) {
            PACKAGE_CHECK(
                (file_name_cell->type == cell_type_e::STRING),
                "file name in pkg's source_files list must be of type `string`")

            {
               auto file_actual = root;
               file_actual /= *file_name_cell->data.s;

               PACKAGE_CHECK(
                   (std::filesystem::is_regular_file(file_actual)),
                   sauros::format(
                       "given source_files target: %  for pkg % does not exist",
                       file_actual.string(), target))

               package.source_file_list.push_back(file_actual);
            }

            // std::cout << "\t[source file] " << *file_name_cell->data.s
            //    << std::endl;
         }
      }

   } // End anon scope

   return package;
}

} // namespace package
} // namespace sauros