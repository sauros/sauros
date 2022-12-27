cmake_minimum_required(VERSION 3.5)
project (fmt)

set(CMAKE_CXX_STANDARD 20)

add_library(fmt_lib SHARED fmt.cpp)
set_target_properties(fmt_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "fmt")
