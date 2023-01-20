cmake_minimum_required(VERSION 3.5)
project (signal)

set(CMAKE_CXX_STANDARD 20)

add_library(signal_lib SHARED signal.cpp)
set_target_properties(signal_lib PROPERTIES PREFIX "" SUFFIX ".lib" OUTPUT_NAME "signal")
