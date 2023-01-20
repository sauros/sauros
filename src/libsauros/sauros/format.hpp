#ifndef SAUROS_FROMAT_HPP
#define SAUROS_FORMAT_HPP

// https://gist.github.com/en4bz/f07ef13706c3ae3a4fb2

#include <sstream>
#include <tuple>

namespace sauros {

template <class Tuple, std::size_t N> struct TuplePrinter {
   static void print(const std::string &fmt, std::ostream &os, const Tuple &t) {
      const size_t idx = fmt.find_last_of('%');
      TuplePrinter<Tuple, N - 1>::print(std::string(fmt, 0, idx), os, t);
      os << std::get<N - 1>(t) << std::string(fmt, idx + 1);
   }
};

template <class Tuple> struct TuplePrinter<Tuple, 1> {
   static void print(const std::string &fmt, std::ostream &os, const Tuple &t) {
      const size_t idx = fmt.find_first_of('%');
      os << std::string(fmt, 0, idx) << std::get<0>(t)
         << std::string(fmt, idx + 1);
   }
};

template <class... Args>
std::string format(const std::string &fmt, Args &&...args) {
   std::stringstream ss;
   const auto t = std::make_tuple(std::forward<Args>(args)...);
   TuplePrinter<decltype(t), sizeof...(Args)>::print(fmt, ss, t);
   return ss.str();
}
} // namespace sauros

#endif