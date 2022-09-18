#ifndef SAUROS_BUFFER_HPP
#define SAUROS_BUFFER_HPP

#include <optional>
#include <string>

namespace sauros {

//! \brief A buffer for data input - This is a helper object that
//!        buffers string input and hands out data when a potentially
//!        valid line of code is ready
class input_buffer_c {
 public:
   //! \brief Add data to the buffer
   //! \returns Optional string - If a string is returned it
   //!          will be a valid line that can be sent to the processor.
   std::optional<std::string> submit(std::string &data);

 private:
   std::string _buffer;
   uint64_t _tracker{0};
};

} // namespace sauros

#endif