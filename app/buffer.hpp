#ifndef APP_BUFFER_HPP
#define APP_BUFFER_HPP

#include <optional>
#include <string>

namespace app {

//! \brief A buffer for data input
class buffer_c {
public:

   //! \brief Add data to the buffer
   //! \returns Optional string - If a string is returned it 
   //!          will be a valid line that can be sent to the processor
   std::optional<std::string> submit(std::string& data);

private:
   std::string _buffer;
   uint64_t _tracker{0};
};


} // namespace app

#endif