#include "signal.hpp"
#include <csignal>
#include <cstdint>
#include <iostream>
#include <sauros/capi/capi.hpp>
#include <unordered_map>

struct cell_env_pair_s {
   sauros::cell_ptr cell;
   std::shared_ptr<sauros::environment_c> env;
};

std::unordered_map<uint8_t, cell_env_pair_s> local_cell_map;

void signal_handler(int sig) {

   // If we don't have a handler set, ignore it
   if (local_cell_map.find(sig) == local_cell_map.end()) {
      return;
   }

   auto active_cell = local_cell_map[sig].cell;
   auto active_env = local_cell_map[sig].env;

   auto list_cell = std::make_shared<sauros::cell_c>(sauros::cell_type_e::LIST,
                                                     active_cell->location);
   list_cell->list.push_back(active_cell);
   list_cell->list.push_back(std::make_shared<sauros::cell_c>(
       sauros::cell_type_e::INTEGER, std::to_string(sig),
       active_cell->location));

   try {
      c_api_process_cell(list_cell, active_env);
   } catch (const std::exception &e) {
      std::cerr << "Signal error > " << e.what() << std::endl;
      std::exit(0);
   }
}

void set_signal_pipe() {
   signal(SIGHUP, signal_handler);  /* Hangup the process */
   signal(SIGINT, signal_handler);  /* Interrupt the process */
   signal(SIGQUIT, signal_handler); /* Quit the process */
   signal(SIGILL, signal_handler);  /* Illegal instruction. */
   signal(SIGTRAP, signal_handler); /* Trace trap. */
   signal(SIGABRT, signal_handler); /* Abort. */
}

sauros::cell_ptr
_pkg_set_signal_handler_all_(sauros::cells_t &cells,
                             std::shared_ptr<sauros::environment_c> env) {
   for (auto i = 1; i < 7; i++) {
      local_cell_map[i] = {cells[1], env};
   }

   set_signal_pipe();

   return std::make_shared<sauros::cell_c>(sauros::CELL_TRUE);
}

sauros::cell_ptr
_pkg_set_signal_handler_(sauros::cells_t &cells,
                         std::shared_ptr<sauros::environment_c> env) {

   // Validated data type in signal.saur
   uint8_t signal_number = c_api_process_cell(cells[1], env)->data.i;
   local_cell_map[signal_number] = {cells[2], env};

   set_signal_pipe();

   return std::make_shared<sauros::cell_c>(sauros::CELL_TRUE);
}
