#ifndef SAUROS_SYSTEM_HPP
#define SAUROS_SYSTEM_HPP

#include <optional>
#include <string>

namespace sauros {

//! \brief Object that detectes sauros home
//!        directory on construction
class system_c {
 public:
   //! \brief Construct the object and attempt to
   //!        determine where the sauros home directory is
   system_c();

   //! \brief Retrieve the home directory
   //! \returns optional string that will be populated
   //!          iff the object was able to detect the home
   //!          directory of sauros AND it exists
   std::optional<std::string> get_sauros_directory() { return _home; }

 private:
   std::optional<std::string> _home;
};

} // namespace sauros

#endif