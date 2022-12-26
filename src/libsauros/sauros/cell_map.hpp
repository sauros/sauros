#ifndef SAUROS_MAP_TYPE_HPP
#define SAUROS_MAP_TYPE_HPP

#include "cell.hpp"
#include "parallel_hashmap/phmap.hpp"
#include <cstdint>

namespace sauros {
using cell_map_t = phmap::parallel_node_hash_map<std::string, cell_ptr>;
} // namespace sauros

#endif