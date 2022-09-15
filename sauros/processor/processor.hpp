#ifndef SAUROS_PROCESSOR_HPP
#define SAUROS_PROCESSOR_HPP

#include "sauros/list.hpp"
#include "sauros/environment.hpp"

#include <unordered_map>
#include <functional>
#include <optional>
#include <vector>
#include <exception>
#include <set>

namespace sauros {

//! \brief A list processor 
class processor_c {
public:

   //!\brief An exception that can be thrown during processing 
   class runtime_exception_c : public std::exception {
   public:
      runtime_exception_c() = delete;

      //! \brief Construct the expception
      //! \param message The message that is to be displayed
      //! \param location The location (line/col) that the error arose
      runtime_exception_c(std::string message, location_s location) : _msg(message), _loc(location) {}

      //! \brief Retrieve the description of the exception
      const char* what() const throw () {
         return _msg.c_str();
      }

      //! \brief Retrieve the location (line/col) that caused the exception to be thrown
      const location_s get_location() { return _loc; }
   private:
      std::string _msg;
      location_s _loc{0,0};
   };

   //! \brief Construct the processor
   processor_c();

   //! \brief Process a cell (generated from parser, or otherwise)
   //! \param cell The cell to process
   //! \param global_env The environment to use 
   //! \returns Optional cell. If a cell is returned, no errors arose
   //! \note Upon an error occuring `runtime_exception_c` will be thrown
   std::optional<cell_c> process(cell_c& cell, std::shared_ptr<environment_c> global_env);

   //! \brief Convert a cell to a string - Something that can be printed to the screen
   //! \param out[out] The string to populate
   //! \param cell[in] The cell to string
   //! \param env The environment to pull cell information from
   //! \param show_space Defaults to true, inserts spaces between items
   void cell_to_string(std::string& out, cell_c& cell, std::shared_ptr<environment_c> env, bool show_space = true);

private:
   std::set<std::string> _key_symbols;
   std::unordered_map<std::string, cell_c> _builtins;

   void populate_builtins();
   std::optional<cell_c>  process_list(std::vector<cell_c>& cells, std::shared_ptr<environment_c> env);
   std::optional<cell_c>  process_cell(cell_c& cell, std::shared_ptr<environment_c> env);
   cell_c perform_arithmetic(std::string op, std::vector<cell_c>& cells, std::function<double(double, double)>fn, std::shared_ptr<environment_c> env, bool force_double = false);
};

} // namespace sauros

#endif