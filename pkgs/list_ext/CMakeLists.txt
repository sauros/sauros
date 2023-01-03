cmake_minimum_required(VERSION 3.5)
project (list_ext)

set(CMAKE_CXX_STANDARD 20)

add_library(list_ext_lib SHARED list_ext.cpp)
set_target_properties(list_ext_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "list_ext")
