#ifndef SAUROS_PKG_STD_FMT
#define SAUROS_PKG_STD_FMT

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
_pkg_fmt_format_encode_(sauros::cells_t &cells,
                        std::shared_ptr<sauros::environment_c> env);

/*
   Encode escaped chars (\n \t etc) into a string with no cells present
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_fmt_format_string_(sauros::cells_t &cells,
                        std::shared_ptr<sauros::environment_c> env);

/*
   Join all elements of a list into a string
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_fmt_join_(sauros::cells_t &cells,
               std::shared_ptr<sauros::environment_c> env);

/*
   Expand a string into a list of individual chars
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_fmt_expand_(sauros::cells_t &cells,
                 std::shared_ptr<sauros::environment_c> env);

/*
   Split a string into a list given a particular delimiter
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_fmt_split_(sauros::cells_t &cells,
                std::shared_ptr<sauros::environment_c> env);
}
#endif