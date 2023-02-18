#include "io.hpp"

#include <iostream>
#include <string>

sauros::cell_ptr _pkg_io_getline_str_(sauros::cells_t &cells,
                                      sauros::env_ptr env) {
   std::string line;
   std::getline(std::cin, line);
   return std::make_shared<sauros::cell_c>(sauros::cell_type_e::STRING, line);
}

sauros::cell_ptr _pkg_io_getline_int_(sauros::cells_t &cells,
                                      sauros::env_ptr env) {

   sauros::cell_int_t x = 0;
   std::string line;
   std::getline(std::cin, line);
   try {
      x = std::stoull(line);
      return std::make_shared<sauros::cell_c>(sauros::cell_type_e::INTEGER, x);
   } catch (std::invalid_argument) {
      return std::make_shared<sauros::cell_c>(sauros::CELL_NIL);
   }
   return std::make_shared<sauros::cell_c>(sauros::CELL_NIL);
}

sauros::cell_ptr _pkg_io_getline_real_(sauros::cells_t &cells,
                                       sauros::env_ptr env) {

   sauros::cell_real_t x = 0.0;
   std::string line;
   std::getline(std::cin, line);
   try {
      x = std::stod(line);
      return std::make_shared<sauros::cell_c>(sauros::cell_type_e::REAL, x);
   } catch (std::invalid_argument) {
      return std::make_shared<sauros::cell_c>(sauros::CELL_NIL);
   }
   return std::make_shared<sauros::cell_c>(sauros::CELL_NIL);
}
