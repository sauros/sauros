#ifndef SAUROS_DRIVER_HPP
#define SAUROS_DRIVER_HPP

#include "environment.hpp"
#include "exceptions.hpp"
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
  driver_if(std::shared_ptr<sauros::environment_c> env);

  //! \brief Mark that fact that no more source is coming in
  void indicate_complete();

protected:
  void execute(const char *source, const uint64_t line_number,
               std::string &line);
  void execute(parser::segment_parser_c::segment_s segment);
  virtual void cell_returned(cell_ptr cell) = 0;
  virtual void except(sauros::exceptions::runtime_c &e) = 0;
  virtual void except(sauros::exceptions::assertion_c &e) = 0;
  virtual void except(sauros::exceptions::unknown_identifier_c &e) = 0;
  virtual void except(sauros::parser::parser_exception_c &e) = 0;

  std::shared_ptr<sauros::environment_c> _env;
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
  file_executor_c(std::shared_ptr<sauros::environment_c> env)
      : driver_if(env) {}

  //! \brief Load and execute the file
  //! \param file The file to laod
  int run(const std::string &file);

  //! \brief Indicate to the file executor that the program
  //!        has finished
  void finish();

private:
  virtual void cell_returned(cell_ptr cell) override final;
  virtual void except(sauros::exceptions::runtime_c &e) override final;
  virtual void except(sauros::exceptions::assertion_c &e) override final;
  virtual void
  except(sauros::exceptions::unknown_identifier_c &e) override final;
  virtual void except(sauros::parser::parser_exception_c &e) override final;
  std::fstream _fs;
  std::string _file;
};

//! \brief REPL
class repl_c : private driver_if {
public:
  //! \brief The maximum lines remembered by repl
  static constexpr size_t MAX_HISTORY_LENGTH = 255;

  repl_c() = delete;

  //! \brief Create the repl object
  //! \param env The environment to use
  repl_c(std::shared_ptr<sauros::environment_c> env) : driver_if(env) {}

  //! \brief Start the REPL
  void start();

  //! \brief Stop the REPL (interrupts etc)
  void stop();

private:
  bool _do{true};
  virtual void cell_returned(cell_ptr cell) override final;
  virtual void except(sauros::exceptions::runtime_c &e) override final;
  virtual void except(sauros::exceptions::assertion_c &e) override final;
  virtual void
  except(sauros::exceptions::unknown_identifier_c &e) override final;
  virtual void except(sauros::parser::parser_exception_c &e) override final;
};

//! \brief REPL
class eval_c : private driver_if {
public:
  eval_c() = delete;

  //! \brief Create the repl object
  //! \param env The environment to use
  //! \param cb The callback to issue when the cell is returned
  eval_c(std::shared_ptr<sauros::environment_c> env,
         std::function<void(cell_ptr cell)> cb)
      : driver_if(env), _cb(cb) {}

  void eval(uint64_t line, std::string data) { execute("eval", line, data); }

private:
  std::function<void(cell_ptr cell)> _cb;
  virtual void cell_returned(cell_ptr cell) override final;
  virtual void except(sauros::exceptions::runtime_c &e) override final;
  virtual void except(sauros::exceptions::assertion_c &e) override final;
  virtual void
  except(sauros::exceptions::unknown_identifier_c &e) override final;
  virtual void except(sauros::parser::parser_exception_c &e) override final;
};
} // namespace sauros

#endif