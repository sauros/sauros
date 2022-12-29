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
} // namespace sauros

#endif