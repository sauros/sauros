#include "io.hpp"

#include <iostream>
#include <string>

sauros::cell_ptr get_str(sauros::cells_t &cells,
                       std::shared_ptr<sauros::environment_c> env) {
   std::string line;
   std::getline(std::cin, line);
   return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, line);
}

sauros::cell_ptr get_int(sauros::cells_t &cells,
                       std::shared_ptr<sauros::environment_c> env) {

   int64_t x = 0;
   std::string line;
   std::getline(std::cin, line);
   try {
      x = std::stoull(line);
      return std::make_shared<sauros::cell_c>(sauros::cell_type_e::INTEGER, std::to_string(x));
   } catch (std::invalid_argument) {
      return std::make_shared<sauros::cell_c>(sauros::CELL_NIL);
   }
   return std::make_shared<sauros::cell_c>(sauros::CELL_NIL);
}

sauros::cell_ptr get_double(sauros::cells_t &cells,
                          std::shared_ptr<sauros::environment_c> env) {

   double x = 0.0;
   std::string line;
   std::getline(std::cin, line);
   try {
      x = std::stod(line);
      return std::make_shared<sauros::cell_c>(sauros::cell_type_e::DOUBLE, std::to_string(x));
   } catch (std::invalid_argument) {
      return std::make_shared<sauros::cell_c>(sauros::CELL_NIL);
   }
   return std::make_shared<sauros::cell_c>(sauros::CELL_NIL);
}
