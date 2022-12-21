#ifndef SAUROS_MAP_TYPE_HPP
#define SAUROS_MAP_TYPE_HPP

#include <cstdint>

#include "third_party/parallel_hashmap/phmap.h"

namespace sauros {
class cell_c;
using cell_map_t = phmap::parallel_node_hash_map<std::string, cell_c>;
} // namespace sauros

#endif