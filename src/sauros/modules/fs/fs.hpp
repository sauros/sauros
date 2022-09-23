#ifndef SAUROUS_MODULES_FS_HPP
#define SAUROUS_MODULES_FS_HPP

#include "../module_if.hpp"

/*
   This is a very basic filesystem module - it needs to be updated 
   if its expected to actually be used beyond the most basic
   of activities
*/

namespace sauros {
namespace modules {

//! \brief FS module
class fs_c : public module_if {
public:
   fs_c();
};

} // namespace modules
} // namespace sauros

#endif