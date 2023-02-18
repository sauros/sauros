#ifndef SAUROS_C_API
#define SAUROS_C_API

#include "sauros/sauros.hpp"

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

API_EXPORT
extern sauros::cell_ptr c_api_process_cell(sauros::cell_ptr cell,
                                           sauros::env_ptr env);

API_EXPORT
extern sauros::cell_ptr c_api_cell_to_string(sauros::cell_ptr cell,
                                             sauros::env_ptr env);
}
#endif