#ifndef SAUROS_PKG_FILE_IO
#define SAUROS_PKG_FILE_IO

#include <sauros/sauros.hpp>

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

/*
   Encode cells into a string
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_file_io_get_handle_(sauros::cells_t &cells,
                         std::shared_ptr<sauros::environment_c> env);
}
#endif