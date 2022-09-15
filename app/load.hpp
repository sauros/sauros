#ifndef APP_LOAD_HPP
#define APP_LOAD_HPP

#include "buffer.hpp"
#include "sauros/sauros.hpp"
#include <string>

namespace app {

class load_c {

public:
   int run(const std::string& file, std::shared_ptr<sauros::environment_c> env);

private:
   buffer_c _buffer;
   sauros::processor_c _list_processor;
};

}

#endif