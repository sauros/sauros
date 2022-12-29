#include "os.hpp"

#include <bit>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <sauros/capi/capi.hpp>
#include <string>

using command = std::function<bool(std::string)>;

/*
   A user could send a single item to work with, or they could
   send us a list. This function flattens lists to a list we can
   operate with
*/
sauros::cells_t populate_source(sauros::cells_t &cells, size_t cell_sources,
                                std::shared_ptr<sauros::environment_c> env) {
   auto raw_source = c_api_process_cell(cells[cell_sources], env);
   sauros::cells_t source_cells;
   {
      auto source = raw_source;
      if (source->type == sauros::cell_type_e::LIST) {
         source_cells.reserve(source->list.size());
         for (auto &c : source->list) {
            source_cells.push_back(c_api_cell_to_string(c, env));
         }
      } else {
         source_cells.push_back(source);
      }
   }
   return source_cells;
}

sauros::cell_ptr execute_commands(sauros::cells_t &cells,
                                  std::shared_ptr<sauros::environment_c> env,
                                  const std::string cmd_name, command cmd) {
   sauros::cells_t sources = populate_source(cells, 1, env);

   auto result = std::make_shared<sauros::cell_c>(sauros::cell_type_e::LIST);
   for (auto &s : sources) {

      auto current_dir =
          std::make_shared<sauros::cell_c>(sauros::cell_type_e::LIST);

      if (s->type != sauros::cell_type_e::STRING) {
         throw sauros::processor_c::runtime_exception_c(
             cmd_name + " command expectes strings for all items",
             cells[0]->location);
      }
      // Indicate the path being listed
      current_dir->list.push_back(s);

      if (!cmd(s->data)) {
         current_dir->list.push_back(
             std::make_shared<sauros::cell_c>(sauros::CELL_FALSE));
      } else {
         current_dir->list.push_back(
             std::make_shared<sauros::cell_c>(sauros::CELL_TRUE));
      }

      result->list.push_back(current_dir);
   }

   return result;
}

sauros::cell_ptr _pkg_os_cwd_(sauros::cells_t &cells,
                              std::shared_ptr<sauros::environment_c> env) {
   return std::make_shared<sauros::cell_c>(
       sauros::cell_type_e::STRING, std::filesystem::current_path().string());
}

sauros::cell_ptr _pkg_os_ls_(sauros::cells_t &cells,
                             std::shared_ptr<sauros::environment_c> env) {
   // If they didn't give use anything other than ls we assume its the cwd
   // otherwise we need to populate a list of them so we treat them as a list
   // throughout
   sauros::cells_t sources = populate_source(cells, 1, env);

   auto result = std::make_shared<sauros::cell_c>(sauros::cell_type_e::LIST);
   for (auto &s : sources) {

      auto current_dir =
          std::make_shared<sauros::cell_c>(sauros::cell_type_e::LIST);

      if (s->type != sauros::cell_type_e::STRING) {
         throw sauros::processor_c::runtime_exception_c(
             "ls command expectes strings for all sources to list",
             cells[0]->location);
      }
      // Indicate the path being listed
      current_dir->list.push_back(s);

      if (!std::filesystem::is_directory(s->data)) {
         current_dir->list.push_back(
             std::make_shared<sauros::cell_c>(sauros::CELL_NIL));
         result->list.push_back(current_dir);
         continue;
      }

      // Now indicate the
      for (const auto &entry : std::filesystem::directory_iterator(s->data)) {
         current_dir->list.push_back(std::make_shared<sauros::cell_c>(
             sauros::cell_type_e::STRING, entry.path().string()));
      }
      result->list.push_back(current_dir);
   }

   return result;
}

sauros::cell_ptr _pkg_os_chdir_(sauros::cells_t &cells,
                                std::shared_ptr<sauros::environment_c> env) {

   auto raw_dest = c_api_process_cell(cells[1], env);
   if (raw_dest->type != sauros::cell_type_e::STRING) {
      throw sauros::processor_c::runtime_exception_c(
          "chdir command expects parameter to be a string", cells[0]->location);
   }

   if (!std::filesystem::is_directory(raw_dest->data)) {
      return std::make_shared<sauros::cell_c>(sauros::CELL_FALSE);
   }

   auto dest = std::filesystem::path(raw_dest->data);
   std::filesystem::current_path(dest);

   auto path = std::filesystem::current_path();

   // Ensure we changed paths
   if (path == dest) {
      return std::make_shared<sauros::cell_c>(sauros::CELL_TRUE);
   }

   return std::make_shared<sauros::cell_c>(sauros::CELL_FALSE);
}

sauros::cell_ptr _pkg_os_endian_(sauros::cells_t &cells,
                                 std::shared_ptr<sauros::environment_c> env) {
   if constexpr (std::endian::native == std::endian::big) {
      return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING,
                                              "big");
   } else if constexpr (std::endian::native == std::endian::little) {
      return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING,
                                              "little");
   } else {
      return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING,
                                              "mixed");
   }
}

sauros::cell_ptr _pkg_os_os_name_(sauros::cells_t &cells,
                                  std::shared_ptr<sauros::environment_c> env) {
#ifdef _WIN32
   return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING,
                                           "windows-32");
   ;
#elif _WIN64
   return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING,
                                           "windows-64");
#elif __APPLE__ || __MACH__
   return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, "mac");
#elif __linux__
   return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING,
                                           "linux");
#elif __FreeBSD__
   return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING,
                                           "free-bsd");
#elif __unix || __unix__
   return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, "unix");
#else
   return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING,
                                           "unknown");
#endif
}

sauros::cell_ptr _pkg_os_is_file_(sauros::cells_t &cells,
                                  std::shared_ptr<sauros::environment_c> env) {

   auto raw_dest = c_api_process_cell(cells[1], env);
   if (raw_dest->type != sauros::cell_type_e::STRING) {
      throw sauros::processor_c::runtime_exception_c(
          "is_file command expects parameter to be a string",
          cells[0]->location);
   }

   if (std::filesystem::is_block_file(raw_dest->data) ||
       std::filesystem::is_regular_file(raw_dest->data) ||
       std::filesystem::is_character_file(raw_dest->data)) {
      return std::make_shared<sauros::cell_c>(sauros::CELL_TRUE);
   }
   return std::make_shared<sauros::cell_c>(sauros::CELL_FALSE);
}

sauros::cell_ptr _pkg_os_is_dir_(sauros::cells_t &cells,
                                 std::shared_ptr<sauros::environment_c> env) {

   auto raw_dest = c_api_process_cell(cells[1], env);
   if (raw_dest->type != sauros::cell_type_e::STRING) {
      throw sauros::processor_c::runtime_exception_c(
          "is_dir command expects parameter to be a string",
          cells[0]->location);
   }

   if (std::filesystem::is_directory(raw_dest->data)) {
      return std::make_shared<sauros::cell_c>(sauros::CELL_TRUE);
   }
   return std::make_shared<sauros::cell_c>(sauros::CELL_FALSE);
}

sauros::cell_ptr _pkg_os_exists_(sauros::cells_t &cells,
                                 std::shared_ptr<sauros::environment_c> env) {

   auto raw_dest = c_api_process_cell(cells[1], env);
   if (raw_dest->type != sauros::cell_type_e::STRING) {
      throw sauros::processor_c::runtime_exception_c(
          "is_file command expects parameter to be a string",
          cells[0]->location);
   }

   if (std::filesystem::exists(raw_dest->data)) {
      return std::make_shared<sauros::cell_c>(sauros::CELL_TRUE);
   }
   return std::make_shared<sauros::cell_c>(sauros::CELL_FALSE);
}

sauros::cell_ptr _pkg_os_mkdir_(sauros::cells_t &cells,
                                std::shared_ptr<sauros::environment_c> env) {
   return execute_commands(cells, env, "mkdir", [](std::string target) -> bool {
      return std::filesystem::create_directory(target);
   });
}

sauros::cell_ptr _pkg_os_delete_(sauros::cells_t &cells,
                                 std::shared_ptr<sauros::environment_c> env) {
   return execute_commands(cells, env, "delete",
                           [](std::string target) -> bool {
                              try {
                                 return std::filesystem::remove(target);
                              } catch (...) {
                                 return false;
                              }
                           });
}

sauros::cell_ptr
_pkg_os_delete_all_(sauros::cells_t &cells,
                    std::shared_ptr<sauros::environment_c> env) {
   return execute_commands(cells, env, "delete_all",
                           [](std::string target) -> bool {
                              return std::filesystem::remove_all(target);
                           });
}

sauros::cell_ptr _pkg_os_copy_(sauros::cells_t &cells,
                               std::shared_ptr<sauros::environment_c> env) {

   auto source = c_api_process_cell(cells[1], env);
   if (source->type != sauros::cell_type_e::STRING) {
      throw sauros::processor_c::runtime_exception_c(
          "copy command expects source parameter to be a string",
          cells[1]->location);
   }

   if (!std::filesystem::exists(source->data)) {
      return std::make_shared<sauros::cell_c>(sauros::CELL_FALSE);
   }

   auto dest = c_api_process_cell(cells[2], env);
   if (dest->type != sauros::cell_type_e::STRING) {
      throw sauros::processor_c::runtime_exception_c(
          "copy command expects destination parameter to be a string",
          cells[2]->location);
   }

   auto flags = c_api_process_cell(cells[3], env);
   if (flags->type != sauros::cell_type_e::BOX) {
      throw sauros::processor_c::runtime_exception_c(
          "copy command expects flags parameter to be a box",
          cells[3]->location);
   }

   if (!flags->box_env->exists("recursive") ||
       !flags->box_env->exists("update_existing") ||
       !flags->box_env->exists("directories_only")) {
      throw sauros::processor_c::runtime_exception_c(
          "flags box missing required members (recursive, update_existing, "
          "directories_only)",
          cells[3]->location);
   }

   auto recursion_cell = flags->box_env->get("recursive");
   auto update_cell = flags->box_env->get("update_existing");
   auto dir_cell = flags->box_env->get("directories_only");

   auto recursion_processed =
       c_api_process_cell(recursion_cell, flags->box_env);
   auto update_processed = c_api_process_cell(update_cell, flags->box_env);
   auto dir_processed = c_api_process_cell(dir_cell, flags->box_env);

   if (recursion_processed->type != sauros::cell_type_e::INTEGER ||
       update_processed->type != sauros::cell_type_e::INTEGER ||
       dir_processed->type != sauros::cell_type_e::INTEGER) {
      throw sauros::processor_c::runtime_exception_c(
          "all copy flags must be an integer type", cells[3]->location);
   }

   std::filesystem::copy_options options;
   if (recursion_processed->data != "0") {
      options |= std::filesystem::copy_options::recursive;
   }
   if (update_processed->data != "0") {
      options |= std::filesystem::copy_options::update_existing;
   }
   if (dir_processed->data != "0") {
      options |= std::filesystem::copy_options::directories_only;
   }

   std::filesystem::copy(source->data, dest->data, options);

   return std::make_shared<sauros::cell_c>(sauros::CELL_TRUE);
}

sauros::cell_ptr
_pkg_os_file_append_(sauros::cells_t &cells,
                     std::shared_ptr<sauros::environment_c> env) {
   auto file = c_api_process_cell(cells[1], env);
   if (file->type != sauros::cell_type_e::STRING) {
      throw sauros::processor_c::runtime_exception_c(
          "file operation expects file name to be a string",
          cells[1]->location);
   }

   std::ofstream out_file;
   out_file.open(file->data, std::ios::out | std::ios::app);

   if (!out_file.is_open()) {
      return std::make_shared<sauros::cell_c>(sauros::CELL_FALSE);
   }

   sauros::cells_t lines = populate_source(cells, 2, env);

   for (auto &line : lines) {
      out_file << line->data;
   }

   out_file.close();

   return std::make_shared<sauros::cell_c>(sauros::CELL_TRUE);
}

sauros::cell_ptr
_pkg_os_file_write_(sauros::cells_t &cells,
                    std::shared_ptr<sauros::environment_c> env) {
   auto file = c_api_process_cell(cells[1], env);
   if (file->type != sauros::cell_type_e::STRING) {
      throw sauros::processor_c::runtime_exception_c(
          "file operation expects file name to be a string",
          cells[1]->location);
   }

   std::ofstream out_file;
   out_file.open(file->data, std::ios::out | std::ios::trunc);

   if (!out_file.is_open()) {
      return std::make_shared<sauros::cell_c>(sauros::CELL_FALSE);
   }

   sauros::cells_t lines = populate_source(cells, 2, env);

   for (auto &line : lines) {
      out_file << line->data;
   }

   out_file.close();

   return std::make_shared<sauros::cell_c>(sauros::CELL_TRUE);
}

sauros::cell_ptr
_pkg_os_file_read_(sauros::cells_t &cells,
                   std::shared_ptr<sauros::environment_c> env) {
   auto file = c_api_process_cell(cells[1], env);
   if (file->type != sauros::cell_type_e::STRING) {
      throw sauros::processor_c::runtime_exception_c(
          "file operation expects file name to be a string",
          cells[1]->location);
   }

   std::ifstream in_file;
   in_file.open(file->data, std::ios::in);

   if (!in_file.is_open()) {
      return std::make_shared<sauros::cell_c>(sauros::CELL_FALSE);
   }

   auto result = std::make_shared<sauros::cell_c>(sauros::cell_type_e::LIST);

   std::string line;
   while (std::getline(in_file, line)) {
      result->list.push_back(
          std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, line));
   }
   in_file.close();
   return result;
}