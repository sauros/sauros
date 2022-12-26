cmake_minimum_required(VERSION 3.5)
project (random)

add_library(random_lib SHARED random.cpp)
set_target_properties(random_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "random")
