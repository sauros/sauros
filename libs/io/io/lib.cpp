#include "lib.hpp"

#include <iostream>
#include <string>

std::optional<sauros::cell_c> get_str
   (std::vector<sauros::cell_c> &cells, std::shared_ptr<sauros::environment_c> env) {
      std::string line;
      std::getline(std::cin, line);
      return {sauros::cell_c(sauros::cell_type_e::STRING, line)};
}

std::optional<sauros::cell_c> get_int
   (std::vector<sauros::cell_c> &cells, std::shared_ptr<sauros::environment_c> env) {

      int64_t x = 0;
      std::string line;
      std::getline(std::cin, line);
      try {
         x = std::stoull(line);
         return {sauros::cell_c(sauros::cell_type_e::INTEGER, std::to_string(x))};
      } catch (std::invalid_argument) {
         return {sauros::CELL_NIL};
      }
      return {sauros::CELL_NIL};
}

std::optional<sauros::cell_c> get_double
   (std::vector<sauros::cell_c> &cells, std::shared_ptr<sauros::environment_c> env) {

      double x = 0.0;
      std::string line;
      std::getline(std::cin, line);
      try {
         x = std::stod(line);
         return {sauros::cell_c(sauros::cell_type_e::DOUBLE, std::to_string(x))};
      } catch (std::invalid_argument) {
         return {sauros::CELL_NIL};
      }
      return {sauros::CELL_NIL};
}
