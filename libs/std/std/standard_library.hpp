#ifndef SAUROS_LIB_STD_HPP
#define SAUROS_LIB_STD_HPP

#include <sauros/sauros.hpp>

#ifdef WIN32
    #define API_EXPORT __declspec(dllexport)
#else 
    #define API_EXPORT 
#endif

extern "C" {

/*
   Print a cell 
*/
API_EXPORT 
extern std::optional<sauros::cell_c> print_cell
   (std::vector<sauros::cell_c> &cells, 
      std::shared_ptr<sauros::environment_c> env);

}


#endif