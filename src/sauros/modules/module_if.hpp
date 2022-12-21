#ifndef SAUROS_MODULE_IF
#define SAUROS_MODULE_IF

#include "../cell.hpp"
#include "third_party/parallel_hashmap/phmap.h"

namespace sauros {

//! \brief An interface for a module
class module_if {
 public:
   virtual ~module_if() {}

   //! \brief Retrieve the map of cells that are contained by the module
   phmap::parallel_node_hash_map<std::string, cell_c> get_members() const {
      return _members_map;
   }

 protected:
   phmap::parallel_node_hash_map<std::string, cell_c> _members_map;

   cell_c load(cell_c &cell, std::shared_ptr<environment_c> env);
};

} // namespace sauros

#endif