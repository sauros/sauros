#include "time.hpp"

#include <chrono>
#include <ctime>
#include <iostream>
#include <sauros/capi/capi.hpp>

inline uint64_t get_stamp() {
   return std::chrono::duration_cast<std::chrono::milliseconds>(
              std::chrono::system_clock::now().time_since_epoch())
       .count();
}

uint64_t diff(sauros::cells_t &cells,
              std::shared_ptr<sauros::environment_c> env) {
   auto raw_start = c_api_process_cell(cells[1], env);
   if (raw_start->type != sauros::cell_type_e::INTEGER) {
      throw sauros::processor_c::runtime_exception_c(
          "diff command requires start time to be an integer",
          cells[1]->location);
   }
   auto raw_end = c_api_process_cell(cells[2], env);
   if (raw_end->type != sauros::cell_type_e::INTEGER) {
      throw sauros::processor_c::runtime_exception_c(
          "diff command requires end time to be an integer",
          cells[2]->location);
   }

   auto start_actual = std::stoull(raw_start->data);
   auto end_actual = std::stoull(raw_end->data);

   if (start_actual >= end_actual) {
      return 0;
   }
   return end_actual - start_actual;
}

sauros::cell_ptr _pkg_time_stamp_(sauros::cells_t &cells,
                                  std::shared_ptr<sauros::environment_c> env) {
   return std::make_shared<sauros::cell_c>(sauros::cell_type_e::INTEGER,
                                           std::to_string(get_stamp()));
}

sauros::cell_ptr
_pkg_time_diff_sec_(sauros::cells_t &cells,
                    std::shared_ptr<sauros::environment_c> env) {
   return std::make_shared<sauros::cell_c>(
       sauros::cell_type_e::INTEGER, std::to_string(diff(cells, env) / 1000));
}

sauros::cell_ptr
_pkg_time_stamp_to_utc_(sauros::cells_t &cells,
                        std::shared_ptr<sauros::environment_c> env) {
   auto raw_time = c_api_process_cell(cells[1], env);
   if (raw_time->type != sauros::cell_type_e::INTEGER) {
      throw sauros::processor_c::runtime_exception_c(
          "time must be an integer for stamp_to_utc", cells[1]->location);
   }
   auto time_actual = std::stoull(raw_time->data);
   time_t time = time_actual / 1000;
   std::string ts = std::ctime(&time);
   return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, ts);
}

sauros::cell_ptr
_pkg_time_get_utc_(sauros::cells_t &cells,
                   std::shared_ptr<sauros::environment_c> env) {
   time_t time = get_stamp() / 1000;
   std::string ts = std::ctime(&time);
   return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, ts);
}
