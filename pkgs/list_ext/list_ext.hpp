#ifndef SAUROS_PKG_LIST_EXT
#define SAUROS_PKG_LIST_EXT

#include <sauros/sauros.hpp>

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

API_EXPORT
extern sauros::cell_ptr
_pkg_list_ext_make_assigned_(sauros::cells_t &cells,
                             std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_pkg_list_ext_sort_(sauros::cells_t &cells,
                    std::shared_ptr<sauros::environment_c> env);
}

#endif