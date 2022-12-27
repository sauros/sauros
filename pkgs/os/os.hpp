#ifndef SAUROS_PKG_OS
#define SAUROS_PKG_OS

#include <sauros/sauros.hpp>

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

/*
   Get the current working directory
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_os_cwd_(sauros::cells_t &cells,
                     std::shared_ptr<sauros::environment_c> env);

}

#endif