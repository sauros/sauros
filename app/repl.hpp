#ifndef SAUROS_APP_REPL_HPP
#define SAUROS_APP_REPL_HPP

#include "buffer.hpp"
#include "sauros/sauros.hpp"
#include <functional>

namespace app {

//! \brief REPL
class repl_c {
 public:
   //! \brief Create the repl object
   //! \param env The environment to use
   repl_c(std::shared_ptr<sauros::environment_c> env) : _env(env) {}

   //! \brief Start the REPL
   void start();

   //! \brief Stop the REPL (interrupts etc)
   void stop();

 private:
   std::shared_ptr<sauros::environment_c> _env;
   sauros::processor_c proc;
   bool _do{true};
   buffer_c _buffer;
   void run(const uint64_t line_number, std::string &line);
};

} // namespace app

#endif