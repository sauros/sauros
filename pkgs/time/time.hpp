#ifndef SAUROS_PKG_TIME
#define SAUROS_PKG_TIME

#include <sauros/sauros.hpp>

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

API_EXPORT
extern sauros::cell_ptr
_pkg_time_stamp_(sauros::cells_t &cells,
                 std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_pkg_time_diff_sec_(sauros::cells_t &cells,
                    std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_pkg_time_stamp_to_utc_(sauros::cells_t &cells,
                        std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_pkg_time_get_utc_(sauros::cells_t &cells,
                   std::shared_ptr<sauros::environment_c> env);
}

#endif