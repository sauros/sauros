#ifndef SAUROS_TYPES_HPP
#define SAUROS_TYPES_HPP

#include "parallel_hashmap/phmap.hpp"
#include <cstdint>
#include <string>

namespace sauros {

class cell_c;
using cell_ptr = std::shared_ptr<cell_c>;
using cells_t = std::vector<cell_ptr>;
using cell_int_t = int64_t;
using cell_real_t = double;
using cell_string_t = std::string;
using cell_map_t = phmap::parallel_node_hash_map<std::string, cell_ptr>;

//! \brief A location representation
struct location_s {

   //! \brief Construct a basic location
   location_s() : line(0), col(0) {}

   //! \brief Create a location with a line and col given
   //! \param l The line number
   //! \param c The column number
   location_s(std::size_t l, std::size_t c) : line(l), col(c) {}

   std::size_t line{0};
   std::size_t col{0};
};
} // namespace sauros

#endif