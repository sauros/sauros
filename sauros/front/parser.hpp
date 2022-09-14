#ifndef SAUROS_PARSER_HPP
#define SAUROS_PARSER_HPP

//#include "ast.hpp"
#include "sauros/types.hpp"
#include "sauros/list.hpp"
#include "sauros/environment.hpp"

namespace sauros {
namespace parser {

enum class result_e {
   OKAY,
   ERROR
};

struct product_s {
   result_e result {result_e::OKAY};
   std::shared_ptr<error::error_c> error_info{nullptr};
   cell_c cell;
};

extern product_s parse_line(
   std::shared_ptr<environment_c> env,
   const char* source_descrption, 
   std::size_t line_number, 
   std::string line);

} // namespace parser
} // namespace sauros

#endif