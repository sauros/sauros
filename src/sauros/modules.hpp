#ifndef SAUROS_AAAAMODULES_HPP
#define SAUROS_AAAAMODULES_HPP

#include "cell.hpp"
#include "modules/module_if.hpp"
#include <unordered_map>
#include <vector>

namespace sauros {

class modules_c {
 public:
   modules_c();
   ~modules_c();

   bool contains(const std::string module_name);

   void populate_environment(const std::string module_name,
                             std::shared_ptr<environment_c> &env);

 private:
   std::unordered_map<std::string, module_if *> _modules;
};

} // namespace sauros

#endif