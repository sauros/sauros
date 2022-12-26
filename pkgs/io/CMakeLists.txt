cmake_minimum_required(VERSION 3.5)
project (io)

set(CMAKE_CXX_STANDARD 20)

add_library(io_lib SHARED io.cpp)
set_target_properties(io_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "io")
