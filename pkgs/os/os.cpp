#include "os.hpp"

#include <bit>
#include <iostream>
#include <string>
#include <filesystem>
#include <sauros/capi/capi.hpp>

/*
   A user could send a single item to work with, or they could 
   send us a list. This function flattens lists to a list we can
   operate with
*/
sauros::cells_t populate_source(sauros::cells_t &cells, std::shared_ptr<sauros::environment_c> env) {
   auto raw_source = c_api_process_cell(cells[1], env);
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

sauros::cell_ptr
_pkg_os_cwd_(sauros::cells_t &cells,
                     std::shared_ptr<sauros::environment_c> env) {
   return std::make_shared<sauros::cell_c>(
      sauros::cell_type_e::STRING, std::filesystem::current_path().string());
}

sauros::cell_ptr
_pkg_os_ls_(sauros::cells_t &cells,
                     std::shared_ptr<sauros::environment_c> env) {
   // If they didn't give use anything other than ls we assume its the cwd
   // otherwise we need to populate a list of them so we treat them as a list
   // throughout
   sauros::cells_t sources;
   if (cells.size() == 1) {
      sources.push_back(
         std::make_shared<sauros::cell_c>(
            sauros::cell_type_e::STRING, std::filesystem::current_path().string()
         )
      );
   } else {
      sources = populate_source(cells, env);
   }

   auto result = std::make_shared<sauros::cell_c>(sauros::cell_type_e::LIST);
   for(auto &s : sources) {

      auto current_dir = std::make_shared<sauros::cell_c>(sauros::cell_type_e::LIST);

      if (s->type != sauros::cell_type_e::STRING) {
        throw sauros::processor_c::runtime_exception_c(
          "ls command expectes strings for all sources to list",
          cells[0]->location);
      }
      // Indicate the path being listed
      current_dir->list.push_back(s);

      if (!std::filesystem::is_directory(s->data)) {
         current_dir->list.push_back(
            std::make_shared<sauros::cell_c>(sauros::CELL_NIL)
         );
         result->list.push_back(current_dir);
         continue;
      }

      // Now indicate the 
      for (const auto & entry : std::filesystem::directory_iterator(s->data)) {
         current_dir->list.push_back(
            std::make_shared<sauros::cell_c>(
               sauros::cell_type_e::STRING,
               entry.path().string()
            )
         );
      }
      result->list.push_back(current_dir);
   }

   return result;
}

sauros::cell_ptr
_pkg_os_chdir_(sauros::cells_t &cells,
                     std::shared_ptr<sauros::environment_c> env) {

   auto raw_dest = c_api_process_cell(cells[1], env);
   if (raw_dest->type != sauros::cell_type_e::STRING) {

      std::cout << cells[1]->data << std::endl;
      throw sauros::processor_c::runtime_exception_c(
         "chdir command expects parameter to be a string",
         cells[0]->location);
   }

   if (!std::filesystem::is_directory(raw_dest->data)) {
      return std::make_shared<sauros::cell_c>(
         sauros::CELL_FALSE
      );
   }

   auto dest = std::filesystem::path(raw_dest->data);
   std::filesystem::current_path(dest);

   auto path = std::filesystem::current_path();

   // Ensure we changed paths
   if (path == dest) {
      return std::make_shared<sauros::cell_c>(
         sauros::CELL_TRUE
      );
   }

   return std::make_shared<sauros::cell_c>(
      sauros::CELL_FALSE
   );
}

sauros::cell_ptr
_pkg_os_endian_(sauros::cells_t &cells,
                     std::shared_ptr<sauros::environment_c> env) {
    if constexpr (std::endian::native == std::endian::big) {
        return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, "big");
    }
    else if constexpr (std::endian::native == std::endian::little) {
        return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, "little");
    }
    else { 
        return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, "mixed");
    }
}

sauros::cell_ptr
_pkg_os_os_name_(sauros::cells_t &cells,
                     std::shared_ptr<sauros::environment_c> env) {
#ifdef _WIN32
return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, "windows-32");;
#elif _WIN64
return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, "windows-64");
#elif __APPLE__ || __MACH__
return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, "mac");
#elif __linux__
return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, "linux");
#elif __FreeBSD__
return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, "free-bsd");
#elif __unix || __unix__
return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, "unix");
#else
return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, "unknown");
#endif
}

