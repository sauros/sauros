#include "environment.hpp"

#include <iostream>

namespace sauros {

bool environment_c::exists(const std::string& item) {
   if (_env.find(item) != _env.end()) {
      return true;
   }
   
   if (_parent) {
      return _parent->exists(item);
   }

   return false;
}

environment_c::environment_c(std::vector<cell_c>& params, std::vector<cell_c>& args, std::shared_ptr<environment_c> outer) : _parent(outer) {

   auto arg = args.begin();
   for (auto param = params.begin(); param != params.end(); ++param) {
      std::cout << param->data<< std::endl;
      _env[param->data] = *arg++;
   }
   std::cout << "GOOD\n";
}

void environment_c::set(const std::string& item, cell_c cell) {
   _env[item] = cell;
}

cell_c environment_c::get(const std::string& item) {
   return _env[item];
}

environment_c* environment_c::find(const std::string &var) {

   if (_env.find(var) != _env.end()) {
      return this;
   }
   if (_parent) {
      return _parent->find(var);
   }

   throw unknown_identifier_c(var);
}

} // namespace sauros