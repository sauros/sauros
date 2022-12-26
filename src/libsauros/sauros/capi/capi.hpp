#ifndef SAUROS_C_API
#define SAUROS_C_API

#include "sauros.hpp"

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif


extern "C" {

API_EXPORT
extern sauros::cell_ptr
c_api_process_cell(sauros::cell_ptr cell, std::shared_ptr<sauros::environment_c> env);

}

#endif