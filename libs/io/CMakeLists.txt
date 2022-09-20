cmake_minimum_required(VERSION 3.5)
project (io)
add_library(sau_io_lib SHARED io/lib.cpp)
set_target_properties(sau_io_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "sauros.io")