#ifndef SAUROS_AST_HPP
#define SAUROS_AST_HPP

#include <memory>
#include <string>

namespace sauros {
namespace ast {

enum class node_type_e {
   INTEGER,
   DOUBLE,
   STRING,
   SYMBOL
};

class node_c {
public:
   node_c(std::string data, node_type_e type) : data(data), type(type) {}
   node_c(node_c* left, node_c* right, std::string data) : left(left), right(right), data(data) {}
   std::unique_ptr<node_c> left{nullptr};
   std::unique_ptr<node_c> right{nullptr};
   std::string data;
   node_type_e type;
};

} // namespace ast
} // namespace sauros

#endif