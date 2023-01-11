#include "cell.hpp"
#include "sauros/processor/processor.hpp"
#include <sstream>

#include <iostream>

namespace sauros {

async_cell_c::async_cell_c(location_s *location)
    : variant_cell_c(cell_variant_type_e::ASYNC, location) {

   get_fn = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          return future.get();
       });

   wait_fn = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          future.wait();
          return std::make_shared<cell_c>(CELL_TRUE);
       });
}

thread_cell_c::thread_cell_c(location_s *location)
    : variant_cell_c(cell_variant_type_e::THREAD, location) {

   is_joinable = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          return std::make_shared<cell_c>(cell_type_e::INTEGER,
                                          std::to_string(thread.joinable()));
       });

   join = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          try {
             thread.join();
          } catch (...) {
             throw processor_c::runtime_exception_c("unable to join thread",
                                                    cells[0]);
          }
          return std::make_shared<cell_c>(CELL_TRUE);
       });

   detach = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          thread.detach();
          return std::make_shared<cell_c>(CELL_TRUE);
       });

   get_id = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          std::stringstream ss;
          ss << thread.get_id();
          return std::make_shared<cell_c>(cell_type_e::INTEGER, ss.str());
       });
}

chan_cell_c::chan_cell_c(location_s *location)
    : variant_cell_c(cell_variant_type_e::CHAN, location) {

   put_fn = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          if (cells.size() == 1) {
             return std::make_shared<cell_c>(CELL_FALSE);
          }
          const std::lock_guard<std::mutex> lock(channel_mutex);
          for (auto it = cells.begin() + 1; it != cells.end(); ++it) {
             channel_queue.push(processor->process_cell((*it), env));
          }
          return std::make_shared<cell_c>(CELL_TRUE);
       });

   has_data_fn = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          const std::lock_guard<std::mutex> lock(channel_mutex);
          return std::make_shared<cell_c>(
              cell_type_e::INTEGER,
              std::to_string(static_cast<int64_t>(!channel_queue.empty())));
       });

   get_fn = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          cell_ptr next;
          {
             const std::lock_guard<std::mutex> lock(channel_mutex);
             next = channel_queue.front();
             channel_queue.pop();
          }
          return next;
       });

   drain_fn = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          const std::lock_guard<std::mutex> lock(channel_mutex);
          cell_ptr result = std::make_shared<cell_c>(cell_type_e::LIST);
          while (!channel_queue.empty()) {
             result->list.push_back(channel_queue.front());
             channel_queue.pop();
          }
          return result;
       });
}

ref_cell_c::ref_cell_c(location_s *location)
    : variant_cell_c(cell_variant_type_e::REF, location) {

   put_fn = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          if (cells.size() != 2) {
             throw processor_c::runtime_exception_c(
                 "ref_cell.put expects 1 parameters", cells[0]);
          }
          const std::lock_guard<std::mutex> lock(ref_mut);
          ref_value = cells[1];
          return std::make_shared<cell_c>(CELL_TRUE);
       });

   get_fn = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          if (cells.size() != 1) {
             throw processor_c::runtime_exception_c(
                 "ref_cell.get expects no parameters", cells[0]);
          }
          const std::lock_guard<std::mutex> lock(ref_mut);
          return ref_value;
       });
}

} // namespace sauros