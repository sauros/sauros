#ifndef SAUROS_PKG_RANDOM
#define SAUROS_PKG_RANDOM

#include <sauros/sauros.hpp>

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

API_EXPORT
extern sauros::cell_ptr _pkg_random_string_(sauros::cells_t &cells,
                                            sauros::env_ptr env);

API_EXPORT
extern sauros::cell_ptr _pkg_random_alpha_string_(sauros::cells_t &cells,
                                                  sauros::env_ptr env);

API_EXPORT
extern sauros::cell_ptr _pkg_random_sourced_string_(sauros::cells_t &cells,
                                                    sauros::env_ptr env);

API_EXPORT
extern sauros::cell_ptr _pkg_random_uniform_int_(sauros::cells_t &cells,
                                                 sauros::env_ptr env);

API_EXPORT
extern sauros::cell_ptr _pkg_random_uniform_real_(sauros::cells_t &cells,
                                                  sauros::env_ptr env);
}

#endif