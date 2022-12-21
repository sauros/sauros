#ifndef SAUROS_NODE_HPP
#define SAUROS_NODE_HPP

#include <variant>
#include <functional>
#include <vector>
#include <optional>
#include <memory>
#include <string>

namespace sauros {

class node_c;
class node_env_c;

using list = std::vector<node_c>;
using env_ptr = std::shared_ptr<node_env_c>;
using callable_fn = std::function<std::optional<node_c>(list&, env_ptr)>;

class variable_c {
public:
   variable_c(){}
   variable_c(std::string x) : data(x) {}
   std::string data;
};

class node_c {
public:
   node_c(){};
   node_c(variable_c x) : data(x) {}
   node_c(std::string x) : data(x) {}
   node_c(callable_fn x) : data(x) {}
   node_c(list x) : data(x) {}
   node_c(env_ptr x) : data(x) {}
   node_c(double x) : data(x) {}

   std::variant<variable_c, std::string, double, callable_fn, list, env_ptr> data;
   bool stop_parsing_flag{false};
   location_s loc;
};

} // namespace sauros

#endif