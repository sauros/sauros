#ifndef SAUROS_PKG_STD
#define SAUROS_PKG_STD

#include <sauros/sauros.hpp>

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

API_EXPORT
extern sauros::cell_ptr
_pkg_std_list_make_assigned_(sauros::cells_t &cells,
                             std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_pkg_std_list_sort_(sauros::cells_t &cells,
                    std::shared_ptr<sauros::environment_c> env);
}

#endif