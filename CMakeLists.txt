cmake_minimum_required(VERSION 3.5)

project (libsauros)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_STATIC_LIBRARY_PREFIX "")

#
# Options
#
option(COMPILE_TESTS "Execute unit tests" ON)
option(WITH_ASAN     "Compile with ASAN" OFF)
option(SETUP_SAUROS "Install local sauros data"    OFF)

#
# Setup build type 'Release vs Debug'
#
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

#
# Locate CPPUTest
#
if(DEFINED ENV{CPPUTEST_HOME})
    message(STATUS "Using CppUTest home: $ENV{CPPUTEST_HOME}")
    set(CPPUTEST_INCLUDE_DIRS $ENV{CPPUTEST_HOME}/include)
    set(CPPUTEST_LIBRARIES $ENV{CPPUTEST_HOME}/lib)
    set(CPPUTEST_LDFLAGS CppUTest CppUTestExt)
else()
    find_package(PkgConfig REQUIRED)
    pkg_search_module(CPPUTEST REQUIRED cpputest>=3.8)
    message(STATUS "Found CppUTest version ${CPPUTEST_VERSION}")
endif()

#
# Setup ASAN
#
if(WITH_ASAN)
  set(CMAKE_CX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
  set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif()

include(${CMAKE_SOURCE_DIR}/cmake/SetEnv.cmake)

set(SAUROS_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/sauros/ast.cpp
  ${CMAKE_SOURCE_DIR}/sauros/parser.cpp
)

set(HEADERS
    ${CMAKE_SOURCE_DIR}/sauros/ast.hpp
    ${CMAKE_SOURCE_DIR}/sauros/parser.hpp
    ${CMAKE_SOURCE_DIR}/sauros/types.hpp
    ${CMAKE_SOURCE_DIR}/sauros/version.hpp
)

set(SOURCES
    ${SAUROS_SOURCES}
)

#
# Configure Library
#
include(${CMAKE_SOURCE_DIR}/cmake/LibraryConfig.cmake)

#
# Configure Install
#
include(${CMAKE_SOURCE_DIR}/cmake/InstallConfig.cmake)

#
# Setup sauros app
#
add_executable(sauros 
  app/main.cpp
  ${SAUROS_SOURCES}
)

target_include_directories(sauros
    PUBLIC 
        ${PROJECT_SOURCE_DIR}
)

#
# Install bin
#
install (TARGETS sauros
    DESTINATION bin)

#
# Tests
#
if(COMPILE_TESTS)
  add_subdirectory(tests)
endif()

#
# Copy stdlib to build dir
#
# add_custom_target(copy_assets
#     COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/.sauros ${CMAKE_CURRENT_BINARY_DIR}/.sauros
# )
# add_dependencies(sauros copy_assets)

if(SETUP_SAUROS)
  if(DEFINED ENV{HOME})
    set(STDLIB_HOME $ENV{HOME})
    add_custom_target(copy_stdlib
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/.sauros ${STDLIB_HOME}/.sauros
    )
    add_dependencies(sauros copy_stdlib)
  else()
    message(FATAL_ERROR "The environment variable HOME has not been set - Can not setup sauros")
  endif()
endif()