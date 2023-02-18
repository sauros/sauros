#ifndef SAUROS_ENVIRONMENT_HPP
#define SAUROS_ENVIRONMENT_HPP

#include "cell.hpp"
#include <unordered_map>
#include <vector>

#include <iostream>

#include "RLL/rll_wrapper.hpp"
#include "types.hpp"

namespace sauros {

//! \brief An environment that contains variables / functions / etcs
class environment_c {
 public:
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

   //! \brief Construct an environment
   //! \param outer (Optional) environment pointer that will be
   //!              the "outer" or "upper/parent" environment [super-scope]
   environment_c(std::shared_ptr<environment_c> outer = nullptr)
       : _parent(outer) {}

   //! \brief Construct an environment with items auto populated (used for
   //! lambdas) \param params Named parameter objects \param args The arguments
   //! that the parameters represent \param outer (Optional) environment pointer
   //! that will be
   //!              the "outer" or "upper/parent" environment [super-scope]
   environment_c(cells_t &params, cells_t &args,
                 std::shared_ptr<environment_c> outer);

   //! \brief Check if something exists
   //! \param item The item name to check the existence of
   //! \returns true iff the item exists withing this or super scopes
   bool exists(const std::string &item);

   //! \brief Find the environment that contains the item (current item or a
   //!        parent super scope)
   //! \param item The item to find
   //! \param origin_cell Cell with origin information
   //! \returns environment pointer that the item can be safely retrieved from
   //! using `get` \note If the item is not reachable within the current, or
   //! parent scope(s)
   //!       then the exception `unknown_identifier_c` will be thrown
   environment_c *find(const std::string &item, cell_ptr origin_cell);

   //! \brief Get an item
   //! \param item the item to get
   //! \returns A cell if it exits
   //! \note This does not check for existence and can cause
   //!       an OOB error if the item does not exist -
   //!       `exists` or `find` should be used prior to this call
   cell_ptr &get(const std::string &item);

   //! \brief Set an item to a value
   //! \param item The item name to set
   //! \param cell The item data to set
   //! \post There will exist an item that maps the string to the cell
   //! \note If the item already exists, it will be overwritten
   void set(const std::string &item, cell_ptr cell);

   //! \brief Retrieve a copy of the env map
   cell_map_t get_map() const { return _env; }

   //! \brief Check if a package is loaded
   //! \param package The package name to checl
   //! \returns true iff the package was loaded
   bool package_loaded(const std::string &package);

   //! \brief Save a package
   //! \param name THe name of the package
   //! \param lib The library to save
   //! \post package_loaded will return true
   //!       if given the name, and the lib will
   //!       stay alive while the environment is
   //!       still alive
   void save_package(const std::string &name,
                     std::shared_ptr<rll_wrapper_c> lib);

   //! \brief Retrieve the last known good source location.
   //!        Used to help in error recovery
   cell_ptr get_last_good_cell() const { return _last_good_cell; };

   void push_parent(std::shared_ptr<environment_c> outer);

   void dump_env();

 private:
   std::shared_ptr<environment_c> _parent{nullptr};
   cell_map_t _env;
   rll_map _loaded_packages;
   cell_ptr _last_good_cell{nullptr};
};


} // namespace sauros

#endif