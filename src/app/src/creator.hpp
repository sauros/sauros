#ifndef SAUROS_CREATOR_HPP
#define SAUROS_CREATOR_HPP

#include <string>

namespace app {

extern int create_package(std::string &name, bool with_cpp = false);

extern int create_app(std::string &name);
} // namespace app

#endif