cmake_minimum_required(VERSION 3.8)

project(example)

set(CMAKE_CXX_STANDARD 20)

find_package (Threads REQUIRED)

find_package(libsauros REQUIRED)
include_directories(${LIBAUROS_INCLUDE_DIRS})

add_executable(example
               main.cpp)
               
target_link_libraries(example
   PRIVATE
   ${LIBSAUROS_LIBRARIES}
   Threads::Threads)

   
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/example.saur
   DESTINATION ${CMAKE_CURRENT_BINARY_DIR})