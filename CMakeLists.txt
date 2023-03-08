cmake_minimum_required(VERSION 3.5)

project(sauros)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(${PROJECT_SOURCE_DIR}/libsauros)
include_directories(${PROJECT_SOURCE_DIR}/libsauros)

find_package (Threads REQUIRED)

#
# Options
#
option(WITH_ASAN     "Compile with ASAN" OFF)

#
# Setup build type 'Release vs Debug'
#
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

#
# Setup ASAN
#
if(WITH_ASAN)
  set(CMAKE_CX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
  set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif()

#
#  Update version file
#
include("cmake/CheckGit.cmake")

#
# Setup sauros app
#
add_executable(sauros
  src/creator.cpp
  src/dir_loader.cpp
  src/main.cpp
)

target_include_directories(sauros
    PUBLIC 
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(sauros 
   PUBLIC
   libsauros
   dl
   Threads::Threads)

#
# Install bin
#
install (TARGETS sauros
    DESTINATION bin)
