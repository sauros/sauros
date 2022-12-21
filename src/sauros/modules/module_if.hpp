#ifndef SAUROS_MODULE_IF
#define SAUROS_MODULE_IF

#include "sauros/cell.hpp"
#include "sauros/cell_map.hpp"

namespace sauros {

//! \brief An interface for a module
class module_if {
 public:
   virtual ~module_if() {}

   //! \brief Retrieve the map of cells that are contained by the module
   cell_map_t get_members() const { return _members_map; }

 protected:
   cell_map_t _members_map;

   cell_c load(cell_c &cell, std::shared_ptr<environment_c> env);
};

} // namespace sauros

#endif