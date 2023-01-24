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
          "diff command requires start time to be an integer", cells[1]);
   }
   auto raw_end = c_api_process_cell(cells[2], env);
   if (raw_end->type != sauros::cell_type_e::INTEGER) {
      throw sauros::processor_c::runtime_exception_c(
          "diff command requires end time to be an integer", cells[2]);
   }

   if (raw_start->data.i >= raw_end->data.i) {
      return 0;
   }
   return raw_end->data.i - raw_start->data.i;
}

sauros::cell_ptr _pkg_time_stamp_(sauros::cells_t &cells,
                                  std::shared_ptr<sauros::environment_c> env) {
   return std::make_shared<sauros::cell_c>(sauros::cell_type_e::INTEGER,
                                           (sauros::cell_int_t)(get_stamp()));
}

sauros::cell_ptr
_pkg_time_diff_sec_(sauros::cells_t &cells,
                    std::shared_ptr<sauros::environment_c> env) {
   return std::make_shared<sauros::cell_c>(
       sauros::cell_type_e::INTEGER,
       (sauros::cell_int_t)(diff(cells, env) / 1000));
}

sauros::cell_ptr
_pkg_time_stamp_to_utc_(sauros::cells_t &cells,
                        std::shared_ptr<sauros::environment_c> env) {
   auto raw_time = c_api_process_cell(cells[1], env);
   if (raw_time->type != sauros::cell_type_e::INTEGER) {
      throw sauros::processor_c::runtime_exception_c(
          "time must be an integer for stamp_to_utc", cells[1]);
   }
   time_t time = raw_time->data.i / 1000;
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
