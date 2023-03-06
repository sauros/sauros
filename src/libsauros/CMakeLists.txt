cmake_minimum_required(VERSION 3.5)

project (libsauros)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")

#
# Options
#
option(COMPILE_TESTS   "Execute unit tests" ON)
option(WITH_ASAN       "Compile with ASAN" OFF)
option(ENABLE_PROFILER "Enable code profiler" OFF)

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

if(ENABLE_PROFILER)
   add_compile_definitions(PROFILER_ENABLED)
endif()

#
#  Update version file
#
include("cmake/CheckGit.cmake")

include(${CMAKE_SOURCE_DIR}/cmake/SetEnv.cmake)

include_directories("sauros")

if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DNDEBUG)
endif()

set(SAUROS_SOURCES
   ${CMAKE_SOURCE_DIR}/sauros/environment.cpp
   ${CMAKE_SOURCE_DIR}/sauros/cell.cpp
   ${CMAKE_SOURCE_DIR}/sauros/driver.cpp
   ${CMAKE_SOURCE_DIR}/sauros/profiler.cpp
   ${CMAKE_SOURCE_DIR}/sauros/capi/capi.cpp
   ${CMAKE_SOURCE_DIR}/sauros/RLL/rll_wrapper.cpp
   ${CMAKE_SOURCE_DIR}/sauros/front/parser.cpp
   ${CMAKE_SOURCE_DIR}/sauros/package/package.cpp
   ${CMAKE_SOURCE_DIR}/sauros/processor/processor.cpp
   ${CMAKE_SOURCE_DIR}/sauros/processor/processor_load_package.cpp
   ${CMAKE_SOURCE_DIR}/sauros/processor/processor_builtins.cpp
)

set(PARALLEL_HASHMAP_HEADERS
   ${CMAKE_SOURCE_DIR}/sauros/parallel_hashmap/btree.hpp
   ${CMAKE_SOURCE_DIR}/sauros/parallel_hashmap/meminfo.hpp
   ${CMAKE_SOURCE_DIR}/sauros/parallel_hashmap/phmap_base.hpp
   ${CMAKE_SOURCE_DIR}/sauros/parallel_hashmap/phmap_bits.hpp
   ${CMAKE_SOURCE_DIR}/sauros/parallel_hashmap/phmap_config.hpp
   ${CMAKE_SOURCE_DIR}/sauros/parallel_hashmap/phmap_dump.hpp
   ${CMAKE_SOURCE_DIR}/sauros/parallel_hashmap/phmap_fwd_decl.hpp
   ${CMAKE_SOURCE_DIR}/sauros/parallel_hashmap/phmap_utils.hpp
   ${CMAKE_SOURCE_DIR}/sauros/parallel_hashmap/phmap.hpp
)

set(LINENOISE_HEADERS
   ${CMAKE_SOURCE_DIR}/sauros/linenoise/linenoise.hpp
)

set(HEADERS
   ${CMAKE_SOURCE_DIR}/sauros/profiler.hpp
   ${CMAKE_SOURCE_DIR}/sauros/format.hpp
   ${CMAKE_SOURCE_DIR}/sauros/front/parser.hpp
   ${CMAKE_SOURCE_DIR}/sauros/driver.hpp
   ${CMAKE_SOURCE_DIR}/sauros/rang.hpp
   ${CMAKE_SOURCE_DIR}/sauros/capi/capi.hpp
   ${CMAKE_SOURCE_DIR}/sauros/RLL/rll_wrapper.hpp
   ${CMAKE_SOURCE_DIR}/sauros/package/package.hpp
   ${CMAKE_SOURCE_DIR}/sauros/processor/processor.hpp
   ${CMAKE_SOURCE_DIR}/sauros/environment.hpp
   ${CMAKE_SOURCE_DIR}/sauros/cell.hpp
   ${CMAKE_SOURCE_DIR}/sauros/types.hpp
   ${CMAKE_SOURCE_DIR}/sauros/version.hpp
   ${CMAKE_SOURCE_DIR}/sauros/system.hpp
   ${CMAKE_SOURCE_DIR}/sauros/sauros.hpp
   ${CMAKE_SOURCE_DIR}/sauros/RLL/RLL.h
   ${PARALLEL_HASHMAP_HEADERS}
   ${LINENOISE_HEADERS}
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
# Tests
#
if(COMPILE_TESTS)
  add_subdirectory(tests)
endif()