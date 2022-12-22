#include "io.hpp"

#include <iostream>
#include <string>

namespace sauros {
namespace modules {

io_c::io_c() {

   _members_map["get_str"] =
       std::make_shared<cell_c>([this](cells_t &cells,
                     std::shared_ptr<environment_c> env) -> cell_ptr {
          std::string line;
          std::getline(std::cin, line);
          return  std::make_shared<cell_c>(cell_c(sauros::cell_type_e::STRING, line));
       });

   _members_map["get_int"] =
       std::make_shared<cell_c>([this](cells_t &cells,
                     std::shared_ptr<environment_c> env) -> cell_ptr {
          int64_t x = 0;
          std::string line;
          std::getline(std::cin, line);
          try {
             x = std::stoull(line);
             return { std::make_shared<cell_c>(sauros::cell_type_e::INTEGER,
                                    std::to_string(x))};
          } catch (std::invalid_argument) {
             return std::make_shared<cell_c>(CELL_NIL);
          }
          return std::make_shared<cell_c>(CELL_NIL);
       });

   _members_map["get_double"] =
       std::make_shared<cell_c>([this](cells_t &cells,
                     std::shared_ptr<environment_c> env) -> cell_ptr {
          double x = 0.0;
          std::string line;
          std::getline(std::cin, line);
          try {
             x = std::stod(line);
             return { std::make_shared<cell_c>(sauros::cell_type_e::DOUBLE,
                                    std::to_string(x))};
          } catch (std::invalid_argument) {
             return std::make_shared<cell_c>(CELL_NIL);
          }
          return std::make_shared<cell_c>(CELL_NIL);
       });
}

} // namespace modules
} // namespace sauros