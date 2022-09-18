#ifndef SAUROS_ENVIRONMENT_HPP
#define SAUROS_ENVIRONMENT_HPP

#include "sauros/cell.hpp"
#include <unordered_map>
#include <vector>

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
      unknown_identifier_c(std::string identifier) : _id(identifier) {}
      const char *what() const throw() { return "Unknown identifier"; }

      //! \brief Retrieve the identifier that was requested
      std::string get_id() const { return _id; }

    private:
      std::string _id;
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
   environment_c(std::vector<cell_c> &params, std::vector<cell_c> &args,
                 std::shared_ptr<environment_c> outer);

   //! \brief Check if something exists
   //! \param item The item name to check the existence of
   //! \returns true iff the item exists withing this or super scopes
   bool exists(const std::string &item);

   //! \brief Find the environment that contains the item (current item or a
   //! parent super scope) \param item The item to find \returns environment
   //! pointer that the item can be safely retrieved from using `get` \note If
   //! the item is not reachable within the current, or parent scope(s)
   //!       then the exception `unknown_identifier_c` will be thrown
   environment_c *find(const std::string &item);

   //! \brief Get an item
   //! \param item the item to get
   //! \returns A cell if it exits
   //! \note This does not check for existence and can cause
   //!       an OOB error if the item does not exist -
   //!       `exists` or `find` should be used prior to this call
   cell_c get(const std::string &item);

   //! \brief Set an item to a value
   //! \param item The item name to set
   //! \param cell The item data to set
   //! \post There will exist an item that maps the string to the cell
   //! \note If the item already exists, it will be overwritten
   void set(const std::string &item, cell_c cell);

 private:
   std::shared_ptr<environment_c> _parent{nullptr};
   std::unordered_map<std::string, cell_c> _env;
};

} // namespace sauros

#endif