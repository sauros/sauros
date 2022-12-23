cmake_minimum_required(VERSION 3.5)
project (example)

add_library(sau_example_lib SHARED example/lib.cpp)
set_target_properties(sau_example_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "sauros.example")
