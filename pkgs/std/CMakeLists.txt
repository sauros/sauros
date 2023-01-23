cmake_minimum_required(VERSION 3.5)
project (std)

set(CMAKE_CXX_STANDARD 20)

add_library(std_lib SHARED std.cpp)
set_target_properties(std_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "std")
