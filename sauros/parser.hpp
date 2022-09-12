#ifndef SAUROS_PARSER_HPP
#define SAUROS_PARSER_HPP

#include "ast.hpp"
#include "types.hpp"

namespace sauros {
namespace parser {

enum class result_e {
   OKAY,
   ERROR
};

struct product_s {
   result_e result {result_e::OKAY};
   std::unique_ptr<error::error_c> error_info{nullptr};
   std::unique_ptr<ast::node_c> tree{nullptr};
};

extern product_s parse_line(const char* source_descrption, std::size_t line_number, std::string line, bool show_tree = false);

} // namespace parser
} // namespace sauros

#endif