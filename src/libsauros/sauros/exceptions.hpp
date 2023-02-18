#ifndef SAUROS_EXCEPTIONS_HPP
#define SAUROS_EXCEPTIONS_HPP

#include "sauros/cell.hpp"
#include "sauros/environment.hpp"
#include <exception>

namespace sauros {

namespace exceptions {

//!\brief An exception that will be thrown on assertion failure
class assertion_c : public std::exception {
 public:
   assertion_c() = delete;

   //! \brief Construct the expception
   //! \param message The message that is to be displayed
   //! \param location The location (line/col) that the error arose
   assertion_c(std::string label, cell_ptr cell) : _label(label), _cell(cell) {}

   //! \brief Retrieve the description of the exception
   const char *what() const throw() { return _label.c_str(); }

   //! \brief Retrieve the location (line/col) that caused the exception to
   //! be thrown
   const location_s *get_location() const { return _cell->location; }

   //! \brief Retrieve the origin
   const std::shared_ptr<std::string> get_origin() const {
      return _cell->origin;
   }

 private:
   std::string _label;
   cell_ptr _cell;
};

//!\brief An exception that can be thrown during processing
class runtime_c : public std::exception {
 public:
   runtime_c() = delete;

   //! \brief Construct the expception
   //! \param message The message that is to be displayed
   runtime_c(std::string message, cell_ptr cell) : _msg(message), _cell(cell) {}

   //! \brief Retrieve the description of the exception
   const char *what() const throw() { return _msg.c_str(); }

   //! \brief Retrieve the location (line/col) that caused the exception to
   //! be thrown
   const location_s *get_location() const { return _cell->location; }

   //! \brief Retrieve the origin (file) that caused the exception to
   //! be thrown - may be null
   const std::shared_ptr<std::string> get_origin() const {
      return _cell->origin;
   }

 private:
   std::string _msg;
   cell_ptr _cell;
};

//! \brief An exception thrown by the request for an identifier that isn't
//! known
class unknown_identifier_c : public std::exception {
 public:
   unknown_identifier_c() = delete;

   //! \brief Create the exception
   //! \param identifier The identifier that was unknown
   unknown_identifier_c(std::string identifier, cell_ptr cell)
       : _id(identifier), _cell(cell) {}
   const char *what() const throw() { return "Unknown identifier"; }

   //! \brief Retrieve the identifier that was requested
   const std::string get_id() const { return _id; }

   //! \brief Retrieve the location
   const location_s *get_location() const { return _cell->location; }

   //! \brief Retrieve the origin
   const std::shared_ptr<std::string> get_origin() const {
      return _cell->origin;
   }

 private:
   std::string _id;
   cell_ptr _cell;
};

} // namespace exceptions
} // namespace sauros

#endif