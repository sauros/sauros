#include "io.hpp"

#include <iostream>
#include <string>

namespace sauros {
namespace modules {

io_c::io_c() {

   _members_map["get_str"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          std::string line;
          std::getline(std::cin, line);
          return {sauros::cell_c(sauros::cell_type_e::STRING, line)};
       });

   _members_map["get_int"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          int64_t x = 0;
          std::string line;
          std::getline(std::cin, line);
          try {
             x = std::stoull(line);
             return {sauros::cell_c(sauros::cell_type_e::INTEGER,
                                    std::to_string(x))};
          } catch (std::invalid_argument) {
             return {sauros::CELL_NIL};
          }
          return {sauros::CELL_NIL};
       });

   _members_map["get_double"] = cell_c(
       [this](std::vector<cell_c> &cells,
              std::shared_ptr<environment_c> env) -> std::optional<cell_c> {
          double x = 0.0;
          std::string line;
          std::getline(std::cin, line);
          try {
             x = std::stod(line);
             return {sauros::cell_c(sauros::cell_type_e::DOUBLE,
                                    std::to_string(x))};
          } catch (std::invalid_argument) {
             return {sauros::CELL_NIL};
          }
          return {sauros::CELL_NIL};
       });
}

} // namespace modules
} // namespace sauros