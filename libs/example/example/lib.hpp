#ifndef SAUROS_LIB_IO_HPP
#define SAUROS_LIB_IO_HPP

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
extern sauros::cell_ptr get_str(sauros::cells_t &cells,
                              std::shared_ptr<sauros::environment_c> env);

/*
   Get user input integer
*/
API_EXPORT
extern sauros::cell_ptr get_int(sauros::cells_t &cells,
                              std::shared_ptr<sauros::environment_c> env);

/*
   Get user input double
*/
API_EXPORT
extern sauros::cell_ptr get_double(sauros::cells_t &cells,
                                 std::shared_ptr<sauros::environment_c> env);
}

#endif