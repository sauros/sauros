#ifndef SAUROS_PARSER_HPP
#define SAUROS_PARSER_HPP

// #include "ast.hpp"
#include "sauros/cell.hpp"
#include "sauros/environment.hpp"
#include "sauros/types.hpp"

namespace sauros {
namespace parser {

//! \brief The parser tokens
enum class token_e {
   L_BRACKET,
   R_BRACKET,
   SYMBOL,
   INTEGER,
   DOUBLE,
   STRING,
};

//! \brief  A token/ data/ location pair
struct token_s {
   token_e token;
   std::string data;
   location_s location;
};

//! \brief A struct to track brackets
struct bracket_track_s {
   location_s location;
   uint64_t tracker{0};
};

//!\brief An exception that can be thrown during parsing
class parser_exception_c : public std::exception {
 public:
   parser_exception_c() = delete;

   //! \brief Construct the expception
   //! \param message The message that is to be displayed
   //! \param location The location (line/col) that the error arose
   parser_exception_c(std::string message, location_s location)
       : _msg(message), _loc(location) {}

   //! \brief Retrieve the description of the exception
   const char *what() const throw() { return _msg.c_str(); }

   //! \brief Retrieve the location (line/col) that caused the exception to
   //! be thrown
   const location_s get_location() { return _loc; }

 private:
   std::string _msg;
   location_s _loc{0, 0};
};

//! \brief A parser meant to parse segments of code
//!        rather than 1 continuous valid line
class segment_parser_c {
 public:
   //! \brief A code segment
   struct segment_s {
      std::string line;
      std::size_t line_number;
   };

   //! \brief Submit a segment to be analyzed
   //! \returns cell_ptr once completed
   std::optional<cell_ptr> submit(segment_s segment);

   //! \brief External indication that there
   //!        exists no more source
   void indicate_complete();

 private:
   std::vector<token_s> _tokens;
   bracket_track_s _bts;
};

//! \brief Parse a line
extern cell_ptr
parse_line(const char *source_descrption, //! Description of the source of the
                                          //! line (file, REPL, etc)
           std::size_t line_number,       //! The line number being parsed
           std::string line);             //! The line itsself

} // namespace parser
} // namespace sauros

#endif