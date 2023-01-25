#include "dir_loader.hpp"
#include <filesystem>

namespace app {

std::string load_dir(std::shared_ptr<sauros::environment_c> env) {

   std::string app_saur = "app.saur";
   if (!std::filesystem::is_regular_file(app_saur)) {
      std::cerr << "Unable to locate " << app_saur << std::endl;
      std::exit(1);
   }

   // Load the file
   auto app_env = std::make_shared<sauros::environment_c>();
   auto exec = sauros::file_executor_c(app_env);
   exec.run(app_saur);
   exec.finish();

   if (app_env->exists("app_name")) {
      auto app_name = app_env->get("app_name");
      if (sauros::cell_type_e::STRING != app_name->type) {
         std::cerr << "Expected `app_name` to be of type string" << std::endl;
         std::exit(1);
      }
      env->set("@app_name", app_name);
   } else {
      env->set("@app_name", std::make_shared<sauros::cell_c>(
                                 sauros::cell_type_e::STRING, "<NOT SET>"));
   }

   if (app_env->exists("authors")) {
      std::vector<sauros::cell_ptr> authors;
      auto app_authors = app_env->get("authors");
      if (sauros::cell_type_e::STRING == app_authors->type) {
         authors.push_back(app_authors);
      } else if (sauros::cell_type_e::LIST == app_authors->type) {
         for(auto author : app_authors->list) {
            if (sauros::cell_type_e::STRING != author->type) {
               std::cerr << "Expected `author` to be of type string" << std::endl;
               std::exit(1);
            }
            authors.push_back(author);
         }
      } else {
         std::cerr << "Expected `authors` to be of type string or type list" << std::endl;
         std::exit(1);
      }
      auto authors_cell = std::make_shared<sauros::cell_c>(
               sauros::cell_type_e::LIST);
      authors_cell->list = authors;
      env->set("@authors", authors_cell);
   } else {
      env->set("@authors", std::make_shared<sauros::cell_c>(
                                 sauros::cell_type_e::STRING, "<NOT SET>"));
   }

   if (app_env->exists("license")) {
      auto license = app_env->get("license");
      if (sauros::cell_type_e::STRING != license->type) {
         std::cerr << "Expected `license` to be of type string" << std::endl;
         std::exit(1);
      }
      env->set("@license", license);
   } else {
      env->set("@license", std::make_shared<sauros::cell_c>(
                                 sauros::cell_type_e::STRING, "<NOT SET>"));
   }


   if (app_env->exists("app_pkgs")) {
      std::vector<sauros::cell_ptr> app_packages;
      auto app_pkgs = app_env->get("app_pkgs");
      if (sauros::cell_type_e::LIST != app_pkgs->type) {
         std::cerr << "Expected `app_pkgs` to be of type list" << std::endl;
         std::exit(1);
      }
      for(auto pkg : app_pkgs->list) {
         if (sauros::cell_type_e::STRING != app_pkgs->type) {
            std::cerr << "Expected `pkg` item to be of type string" << std::endl;
            std::exit(1);
         }
         app_packages.push_back(pkg);


         // TODO : Check to ensure that 
         //        all of these "app_package" are 
         //        actually there and are valid packages
         //        




         // ALSO TODO: Need to check that `use` will actually 
         //            search locally, and not just in the sauros
         //            home dir




      }
   }



   if (app_env->exists("requires")) {
      std::vector<sauros::cell_ptr> requried_packages;
      auto app_requires = app_env->get("requires");
      if (sauros::cell_type_e::LIST != app_requires->type) {
         std::cerr << "Expected `requires` to be of type list" << std::endl;
         std::exit(1);
      }
      for(auto required : app_requires->list) {
         if (sauros::cell_type_e::STRING != required->type) {
            std::cerr << "Expected `required` item to be of type string" << std::endl;
            std::exit(1);
         }
         requried_packages.push_back(required);
      }

      // TODO : Come up with way to iterate over
      //        packages and check if they exist
      //        this should be done in sauros::packages
      //        as a package manager will need to do this as well











   }

   if (!app_env->exists("entry")) {
      std::cerr << "Expected `entry` in app.saur to inform what file is used to launch the application" << std::endl;
      std::exit(1);
   }

   auto entry_file = app_env->get("entry");
   if (sauros::cell_type_e::STRING != entry_file->type) {
      std::cerr << "Expected `entry` item to be of type string" << std::endl;
      std::exit(1);
   }

   std::filesystem::path entry_actual = entry_file->data_as_str();
   if (!std::filesystem::is_regular_file(entry_actual)) {
      std::cerr << "File set for entry `" << entry_actual.string() << "` does not exist!" << std::endl;
      std::exit(1);
   }

   return entry_actual.string();
}


}