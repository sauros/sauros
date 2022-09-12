#ifndef SAUROS_TYPES_HPP
#define SAUROS_TYPES_HPP

#include <string>
#include <cstdint>

namespace sauros {

struct location_s {
   location_s(std::size_t l, std::size_t c) : line(l), col(c) {}
   std::size_t line{0};
   std::size_t col{0};
};


namespace error {

class error_c {
public:
   error_c(location_s loc, std::string source, std::string message) : location(loc.line, loc.col), source(source), message(message) {}
   location_s location{0,0};
   std::string source;
   std::string message;
};

} // namespace error
} // namespace sauros

#endif