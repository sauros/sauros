#ifndef SAUROS_PKG_MATH
#define SAUROS_PKG_MATH

#include <sauros/sauros.hpp>

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

API_EXPORT
extern sauros::cell_ptr
_sauros_pkg_math_log_(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_sauros_pkg_math_log2_(sauros::cells_t &cells,
                       std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_sauros_pkg_math_log10_(sauros::cells_t &cells,
                        std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_sauros_pkg_math_sin_(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_sauros_pkg_math_cos_(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_sauros_pkg_math_tan_(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_sauros_pkg_math_asin_(sauros::cells_t &cells,
                       std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_sauros_pkg_math_acos_(sauros::cells_t &cells,
                       std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_sauros_pkg_math_atan_(sauros::cells_t &cells,
                       std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_sauros_pkg_math_sinh_(sauros::cells_t &cells,
                       std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_sauros_pkg_math_cosh_(sauros::cells_t &cells,
                       std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_sauros_pkg_math_tanh_(sauros::cells_t &cells,
                       std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_sauros_pkg_math_asinh_(sauros::cells_t &cells,
                        std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_sauros_pkg_math_acosh_(sauros::cells_t &cells,
                        std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_sauros_pkg_math_atanh_(sauros::cells_t &cells,
                        std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_sauros_pkg_math_exp_(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_sauros_pkg_math_sqrt_(sauros::cells_t &cells,
                       std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_sauros_pkg_math_ceil_(sauros::cells_t &cells,
                       std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_sauros_pkg_math_floor_(sauros::cells_t &cells,
                        std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_sauros_pkg_math_abs_(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
_sauros_pkg_math_pow_(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);
}

#endif