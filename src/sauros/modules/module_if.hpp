#ifndef SAUROS_MODULE_IF
#define SAUROS_MODULE_IF

#include "../cell.hpp"
#include <unordered_map>

namespace sauros {

class module_if {
public:
   virtual ~module_if() {}
   std::unordered_map<std::string, cell_c> get_members() const {
      return _members_map;
   }

protected:
   std::unordered_map<std::string, cell_c> _members_map;
};

}

#endif