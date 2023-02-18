#ifndef SAUROS_PKG_IO
#define SAUROS_PKG_IO

#include <sauros/sauros.hpp>

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

/*
   Get user input string
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_io_getline_str_(sauros::cells_t &cells,
                     sauros::env_ptr env);

/*
   Get user input integer
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_io_getline_int_(sauros::cells_t &cells,
                     sauros::env_ptr env);

/*
   Get user input double
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_io_getline_real_(sauros::cells_t &cells,
                      sauros::env_ptr env);
}

#endif