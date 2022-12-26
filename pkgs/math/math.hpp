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

/*
API_EXPORT
extern sauros::cell_ptr
log10(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
sin(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
cos(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
tan(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
asin(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
acos(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
atan(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);
               
API_EXPORT
extern sauros::cell_ptr
sinh(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
cosh(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
tanh(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
asinh(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
acosh(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
atanh(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
exp(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
sqrt(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
ceil(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
floor(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
abs(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

API_EXPORT
extern sauros::cell_ptr
pow(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

*/
}

#endif