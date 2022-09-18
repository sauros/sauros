#ifndef SAUROS_CELL_HPP
#define SAUROS_CELL_HPP

#include "types.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace sauros {

//! \brief Types of cells
enum class cell_type_e {
   SYMBOL,
   LIST,
   LAMBDA,
   STRING,
   INTEGER,
   DOUBLE,
};

//! \brief Retrieve the type as a string 
static const char * cell_type_to_string(const cell_type_e type) {
   switch (type) {
      case cell_type_e::SYMBOL: return "symbol";
      case cell_type_e::LIST: return "list";
      case cell_type_e::LAMBDA: return "lambda";
      case cell_type_e::STRING: return "string";
      case cell_type_e::INTEGER: return "integer";
      case cell_type_e::DOUBLE: return "double";
   }
   return "unknown";
}

//! \brief Forward of environment for proc_f
class environment_c;

//! \brief An cell representation
class cell_c {
 public:
   //! \brief Function pointer definition for a cell
   //!        used to execute code
   using proc_f = std::function<std::optional<cell_c>(
       std::vector<cell_c> &, std::shared_ptr<environment_c> env)>;

   //! \brief Create an empty cell
   //! \param type The type to set (Defaults to SYMBOL)
   cell_c(cell_type_e type = cell_type_e::SYMBOL) {}

   //! \brief Create a standard cell
   //! \param type The type to set
   //! \param data The data to set
   cell_c(cell_type_e type, const std::string &data) : type(type), data(data) {}

   //! \brief Create a standard cell
   //! \param type The type to set
   //! \param data The data to set
   //! \param location The location in source that the cell originated from
   cell_c(cell_type_e type, const std::string &data, location_s location)
       : type(type), data(data), location(location) {}

   //! \brief Create a process cell
   //! \param proc The process function to set
   //! \note Process cells are declared as SYMBOL to reduce number of
   //!       given celltypes. A process cell can be quickly identified
   //!       by the existence of a valid proc_f being set
   cell_c(proc_f proc) : type(cell_type_e::SYMBOL), proc(proc) {}

   //! \brief Create a list cell
   //! \param list The list to set in the cell
   cell_c(std::vector<cell_c> list) : type(cell_type_e::LIST), list(list) {}

   // Data
   std::string data;
   cell_type_e type{cell_type_e::SYMBOL};
   location_s location;
   proc_f proc;
   std::vector<cell_c> list;
   bool stop_processing{false};
};

static const cell_c CELL_TRUE =
    cell_c(cell_type_e::INTEGER, "1"); //! A cell that represents TRUE
static const cell_c CELL_FALSE =
    cell_c(cell_type_e::INTEGER, "0"); //! A cell that represents FALSE
static const cell_c CELL_NIL =
    cell_c(cell_type_e::STRING, "#nil"); //! A cell that represents NIL

} // namespace sauros

#endif