#ifndef SAUROS_TYPES_HPP
#define SAUROS_TYPES_HPP

#include <cstdint>
#include <string>

namespace sauros {

//! \brief A location representation
struct location_s {

   //! \brief Construct a basic location
   location_s() : line(0), col(0) {}

   //! \brief Create a location with a line and col given
   //! \param l The line number
   //! \param c The column number
   location_s(std::size_t l, std::size_t c) : line(l), col(c) {}

   std::size_t line{0};
   std::size_t col{0};
};

namespace error {

//! \brief A general error representation
class error_c {
 public:
   //! \brief Construct the error
   //! \param loc The location that the error arose from
   //! \param source The source description (file/ REPL/ etc)
   //! \param message The message generated for the error
   error_c(location_s loc, std::string source, std::string message)
       : location(loc.line, loc.col), source(source), message(message) {}

   location_s location{0, 0};
   std::string source;
   std::string message;
};

} // namespace error
} // namespace sauros

#endif