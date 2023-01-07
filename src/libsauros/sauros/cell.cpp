#include "cell.hpp"

#include "sauros/processor/processor.hpp"

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

chan_cell_c::chan_cell_c(location_s *location)
    : variant_cell_c(cell_variant_type_e::CHAN, location) {

   put_fn = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          if (cells.size() == 1) {
             return std::make_shared<cell_c>(CELL_FALSE);
          }
          const std::lock_guard<std::mutex> lock(channel_mutex);
          for (auto it = cells.begin() + 1; it != cells.end(); ++it) {
             channel_queue.push((*it));
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
          return processor->process_cell(next, env);
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
} // namespace sauros