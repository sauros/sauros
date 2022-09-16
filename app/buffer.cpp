#include "buffer.hpp"

namespace app {

std::optional<std::string> buffer_c::submit(std::string &line) {

   // Remove comments
   std::size_t comment_loc = line.find_first_of(";");
   if (comment_loc != std::string::npos) {
      line = line.substr(0, comment_loc);
   }

   if (line.empty()) {
      return {};
   }

   // Walk the line and see if the given brackets
   // indicate that we have a full
   for (auto &c : line) {
      if (c == '[') {
         _tracker++;
      } else if (c == ']') {
         _tracker--;
      }
      _buffer += c;
   }

   // If we have a statement and all brackets are closed then
   // we can submit the statement
   if (_tracker == 0 && !_buffer.empty()) {
      std::string ret_buffer = _buffer;
      _buffer.clear();
      return {ret_buffer};
   } else {
      _buffer += ' ';
   }

   return {};
}

} // namespace app