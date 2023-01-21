cmake_minimum_required(VERSION 3.5)
project (file_io)

set(CMAKE_CXX_STANDARD 20)

add_library(file_io_lib SHARED file_io.cpp)
set_target_properties(file_io_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "file_io")
