#ifndef APP_LOAD_HPP
#define APP_LOAD_HPP

#include "buffer.hpp"
#include "sauros/sauros.hpp"
#include <string>

namespace app {

//! \brief A source file loader and executor
class load_c {

 public:
   //! \brief Load and execute the file
   //! \param file The file to laod
   //! \param env The environment to use
   int run(const std::string &file,
           std::shared_ptr<sauros::environment_c> &env);

 private:
   buffer_c _buffer;
   sauros::processor_c _list_processor;
};

} // namespace app

#endif