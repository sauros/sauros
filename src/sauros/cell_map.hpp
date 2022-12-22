#ifndef SAUROS_MAP_TYPE_HPP
#define SAUROS_MAP_TYPE_HPP

#include <cstdint>
#include "cell.hpp"
#include "third_party/parallel_hashmap/phmap.h"

namespace sauros {
using cell_map_t = phmap::parallel_node_hash_map<std::string, cell_ptr>;
} // namespace sauros

#endif