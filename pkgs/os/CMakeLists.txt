cmake_minimum_required(VERSION 3.5)
project (os)

set(CMAKE_CXX_STANDARD 20)


add_library(os_lib SHARED os.cpp)
set_target_properties(os_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "os")
