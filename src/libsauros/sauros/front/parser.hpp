#ifndef SAUROS_PARSER_HPP
#define SAUROS_PARSER_HPP

// #include "ast.hpp"
#include "sauros/cell.hpp"
#include "sauros/environment.hpp"
#include "sauros/types.hpp"
#include <string>

namespace sauros {
namespace parser {

//! \brief The parser tokens
enum class token_e {
   L_BRACKET,
   R_BRACKET,
   SYMBOL,
   INTEGER,
   REAL,
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
   //! \param origin The origin file of the error
   //! \param location The location (line/col) that the error arose
   parser_exception_c(std::string message, std::shared_ptr<std::string> origin,
                      location_s location)
       : _msg(message), _origin(origin), _loc(location) {}

   //! \brief Retrieve the description of the exception
   const char *what() const throw() { return _msg.c_str(); }

   //! \brief Retrieve the location (line/col) that caused the exception to
   //! be thrown
   const location_s *get_location() const { return &_loc; }

   //! \brief Retrieve the origin of the exception
   const std::shared_ptr<std::string> get_origin() const { return _origin; }

 private:
   std::string _msg;
   location_s _loc{0, 0};
   std::shared_ptr<std::string> _origin;
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

   //! \brief Set the origin file the source so
   //!        it can be encoded into the cells
   //! \param origin THe origin of the cells
   void set_origin(const std::string &origin);

 private:
   std::vector<token_s> _tokens;
   bracket_track_s _bts;
   std::shared_ptr<std::string> _origin{nullptr};
};

//! \brief Parse a line
extern cell_ptr
parse_line(const char *source_descrption, //! Description of the source of the
                                          //! line (file, REPL, etc)
           std::size_t line_number,       //! The line number being parsed
           std::string line);             //! The line itsself

//! \brief Remove the comments from a specific line
//! \param line The line to strip comments from
//! \post The line will be directly modified to be without comments
static constexpr void remove_comments(std::string &line) {

   // # lines will be entirely removed, though this way they can still
   // be used in variable names/ etc but still allow for #! at the
   // start of a file
   if (line.size() && line[0] == '#') {
      line.clear();
   }

   // Functioned in case we want more comment tokens
   constexpr auto remove_after = [](const std::size_t idx, std::string &line) {
      if (idx == std::string::npos || idx == 0) {
         return;
      }
      line = line.substr(0, idx);
      return;
   };
   remove_after(line.find_first_of(';'), line);
}
} // namespace parser
} // namespace sauros

#endif