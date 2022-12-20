#ifndef SAUROS_DRIVER_HPP
#define SAUROS_DRIVER_HPP

#include "environment.hpp"
#include "front/parser.hpp"
#include "processor/processor.hpp"
#include <fstream>

namespace sauros {

//! \brief Forward for an input buffer object
class input_buffer_c;

//! \brief Interface for the types of processor drivers (REPL, File, Etc)
class driver_if {
 public:
   driver_if() = delete;
   ~driver_if();

   //! \brief Create the driver
   //! \param env The environment
   driver_if(std::shared_ptr<sauros::environment_c> &env);

 protected:
   void execute(const char *source, const uint64_t line_number,
                std::string &line);
   void execute(parser::segment_parser_c::segment_s segment);
   virtual void cell_returned(std::optional<cell_c> cell) = 0;
   virtual void except(sauros::processor_c::runtime_exception_c &e) = 0;
   virtual void except(sauros::processor_c::assertion_exception_c &e) = 0;
   virtual void except(sauros::environment_c::unknown_identifier_c &e) = 0;
   virtual void parser_error(std::string &e, location_s location) = 0;

   std::shared_ptr<sauros::environment_c> &_env;
   sauros::input_buffer_c *_buffer{nullptr};
   sauros::processor_c _list_processor;
   parser::segment_parser_c _segment_parser;
};

//! \brief A file input object that reads an entire
//!        file into the given environment
class file_executor_c : private driver_if {
 public:
   file_executor_c() = delete;

   //! \brief Create the executor
   //! \param env The environment to use
   file_executor_c(std::shared_ptr<sauros::environment_c> &env)
       : driver_if(env) {}

   //! \brief Load and execute the file
   //! \param file The file to laod
   int run(const std::string &file);

 private:
   virtual void cell_returned(std::optional<cell_c> cell) override final;
   virtual void
   except(sauros::processor_c::runtime_exception_c &e) override final;
   virtual void
   except(sauros::processor_c::assertion_exception_c &e) override final;
   virtual void
   except(sauros::environment_c::unknown_identifier_c &e) override final;
   virtual void parser_error(std::string &e,
                             location_s location) override final;

   std::fstream _fs;
   std::string _file;
};

//! \brief REPL
class repl_c : private driver_if {
 public:
   repl_c() = delete;

   //! \brief Create the repl object
   //! \param env The environment to use
   repl_c(std::shared_ptr<sauros::environment_c> &env) : driver_if(env) {}

   //! \brief Start the REPL
   void start();

   //! \brief Stop the REPL (interrupts etc)
   void stop();

 private:
   bool _do{true};
   virtual void cell_returned(std::optional<cell_c> cell) override final;
   virtual void
   except(sauros::processor_c::runtime_exception_c &e) override final;
   virtual void
   except(sauros::processor_c::assertion_exception_c &e) override final;
   virtual void
   except(sauros::environment_c::unknown_identifier_c &e) override final;
   virtual void parser_error(std::string &e,
                             location_s location) override final;
};

//! \brief REPL
class eval_c : private driver_if {
 public:
   eval_c() = delete;

   //! \brief Create the repl object
   //! \param env The environment to use
   //! \param cb The callback to issue when the cell is returned
   eval_c(std::shared_ptr<sauros::environment_c> &env,
          std::function<void(std::optional<cell_c> cell)> cb)
       : driver_if(env), _cb(cb) {}

   void eval(uint64_t line, std::string data) { execute("eval", line, data); }

 private:
   std::function<void(std::optional<cell_c> cell)> _cb;
   virtual void cell_returned(std::optional<cell_c> cell) override final;
   virtual void
   except(sauros::processor_c::runtime_exception_c &e) override final;
   virtual void
   except(sauros::processor_c::assertion_exception_c &e) override final;
   virtual void
   except(sauros::environment_c::unknown_identifier_c &e) override final;
   virtual void parser_error(std::string &e,
                             location_s location) override final;
};
} // namespace sauros

#endif