#ifndef SAUROS_PARSER_HPP
#define SAUROS_PARSER_HPP

//#include "ast.hpp"
#include "sauros/cell.hpp"
#include "sauros/environment.hpp"
#include "sauros/types.hpp"

namespace sauros {
namespace parser {

//! \brief A result that comes back from the parser
enum class result_e {
   OKAY, //! Parsing of line was a success
   ERROR //! Something went wrong
};

//! \brief Resulting product from parsing a ling
struct product_s {
   result_e result{result_e::OKAY}; //! The result
   std::shared_ptr<error::error_c> error_info{
       nullptr}; // Error information, populated iff the an error happened
   cell_c cell;  //! Resulting cell item from parsing
};

//! \brief Parse a line
extern product_s
parse_line(const char *source_descrption, //! Description of the source of the
                                          //! line (file, REPL, etc)
           std::size_t line_number,       //! The line number being parsed
           std::string line);             //! The line itsself

} // namespace parser
} // namespace sauros

#endif