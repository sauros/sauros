cmake_minimum_required(VERSION 3.5)
project (time)

set(CMAKE_CXX_STANDARD 20)

add_library(time_lib SHARED time.cpp)
set_target_properties(time_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "time")
