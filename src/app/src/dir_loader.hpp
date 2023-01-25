#ifndef SAUROS_DIRECTORY_LOADER_HPP
#define SAUROS_DIRECTORY_LOADER_HPP

#include <sauros/sauros.hpp>
#include <string>

namespace app {

//! \brief Attempts to load information from the current
//!         working directory to launch an application
//! \returns filename to execute in directory when completed
//!          and issues a std::exit on failure
//! \note load_dir expects that the caller has moved the current 
//!       working directory to that of which we are attempting to load
extern std::string load_dir(std::shared_ptr<sauros::environment_c> env);
    
}

#endif