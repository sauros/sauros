#ifndef SAUROS_PROCESSOR_HPP
#define SAUROS_PROCESSOR_HPP

#include "sauros/builtin_encodings.hpp"
#include "sauros/cell.hpp"
#include "sauros/environment.hpp"
#include "sauros/system/system.hpp"

#include "sauros/cell_map.hpp"
#include <array>
#include <exception>
#include <functional>
#include <optional>
#include <set>
#include <vector>

namespace sauros {

//! \brief A list processor
class processor_c {
 public:
   //!\brief An exception that will be thrown on assertion failure
   class assertion_exception_c : public std::exception {
    public:
      assertion_exception_c() = delete;

      //! \brief Construct the expception
      //! \param message The message that is to be displayed
      //! \param location The location (line/col) that the error arose
      assertion_exception_c(std::string label, cell_ptr cell)
          : _label(label), _cell(cell) {}

      //! \brief Retrieve the description of the exception
      const char *what() const throw() { return _label.c_str(); }

      //! \brief Retrieve the location (line/col) that caused the exception to
      //! be thrown
      const location_s get_location() const { return _cell->location; }

      //! \brief Retrieve the origin
      const std::shared_ptr<std::string> get_origin() const {
         return _cell->origin;
      }

    private:
      std::string _label;
      cell_ptr _cell;
   };

   //!\brief An exception that can be thrown during processing
   class runtime_exception_c : public std::exception {
    public:
      runtime_exception_c() = delete;

      //! \brief Construct the expception
      //! \param message The message that is to be displayed
      runtime_exception_c(std::string message, cell_ptr cell)
          : _msg(message), _cell(cell) {}

      //! \brief Retrieve the description of the exception
      const char *what() const throw() { return _msg.c_str(); }

      //! \brief Retrieve the location (line/col) that caused the exception to
      //! be thrown
      const location_s get_location() const { return _cell->location; }

      //! \brief Retrieve the origin (file) that caused the exception to
      //! be thrown - may be null
      const std::shared_ptr<std::string> get_origin() const {
         return _cell->origin;
      }

    private:
      std::string _msg;
      cell_ptr _cell;
   };

   //! \brief Construct the processor
   processor_c();

   //! \brief Destruct the processor
   ~processor_c();

   //! \brief Process a cell (generated from parser, or otherwise)
   //! \param cell The cell to process
   //! \param global_env The environment to use
   //! \returns Optional cell. If a cell is returned, no errors arose
   //! \note Upon an error occuring `runtime_exception_c` will be thrown
   cell_ptr process_cell(cell_ptr cell, std::shared_ptr<environment_c> env);

   //! \brief Convert a cell to a string - Something that can be printed to the
   //! screen \param out[out] The string to populate \param cell[in] The cell to
   //! string \param env The environment to pull cell information from \param
   //! show_space Defaults to true, inserts spaces between items
   void cell_to_string(std::string &out, cell_ptr cell,
                       std::shared_ptr<environment_c> env,
                       bool show_space = false);

   //! \brief Reset the processor
   void reset();

 private:
   sauros::system_c _system;
   std::array<cell_ptr, BUILTIN_ENTRY_COUNT> _builtins;

   void populate_standard_builtins();

   std::vector<std::string> retrieve_accessors(const std::string &value);

   void quote_cell(std::string &out, cell_ptr cell,
                   std::shared_ptr<environment_c> env);

   cell_ptr process_list(cells_t &cells, std::shared_ptr<environment_c> env);

   cell_ptr process_lambda(cell_ptr cell, cells_t &cells,
                           std::shared_ptr<environment_c> env);

   cell_ptr clone_box(cell_ptr cell);

   cell_ptr access_box_member(cell_ptr cell,
                              std::shared_ptr<environment_c> &env);

   cell_ptr perform_arithmetic(std::string op, cells_t &cells,
                               std::function<double(double, double)> fn,
                               std::shared_ptr<environment_c> env,
                               bool force_double = false);

   void load_package(const std::string &target, location_s location,
                     std::shared_ptr<environment_c> env);

   cell_ptr _yield_cell{nullptr};
   processor_c *_sub_processor{nullptr};
};

} // namespace sauros

#endif