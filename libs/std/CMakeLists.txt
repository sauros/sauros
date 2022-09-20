cmake_minimum_required(VERSION 3.5)
project (std)
add_library(sau_std_lib SHARED std/standard_library.cpp)
set_target_properties(sau_std_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "sauros.std")