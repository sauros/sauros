#ifndef SAUROS_LIST_HPP
#define SAUROS_LIST_HPP

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "types.hpp"
#include <optional>

namespace sauros {

class environment_c;

enum class cell_type_e {
   SYMBOL, LIST, LAMBDA, STRING, INTEGER, DOUBLE,
};

class cell_c {
public:
   using proc_f = std::function<std::optional<cell_c>(std::vector<cell_c>&)>;

   cell_c(cell_type_e type = cell_type_e::SYMBOL){}
   cell_c(cell_type_e type, const std::string& data, location_s location, std::shared_ptr<environment_c> env) : type(type), data(data), location(location), env(env){}
   cell_c(proc_f proc, std::shared_ptr<environment_c> env = nullptr) : type(cell_type_e::SYMBOL), proc(proc), env(env) {}
   cell_c(std::vector<cell_c> list) : type(cell_type_e::LIST), list(list) {}

   std::string data;
   cell_type_e type{cell_type_e::SYMBOL};
   location_s location;
   proc_f proc;
   std::vector<cell_c> list;
   std::shared_ptr<environment_c> env {nullptr};
};






} // namespace sauros

#endif