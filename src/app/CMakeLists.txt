cmake_minimum_required(VERSION 3.5)

project(sauros)

set(CMAKE_CXX_STANDARD 20)

find_package(libsauros REQUIRED)
include_directories(${LIBAUROS_INCLUDE_DIRS})

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
# Add third party directories for app
#
add_subdirectory(src/third_party/hwinfo)
#
# Setup sauros app
#
add_executable(sauros
  src/main.cpp
)

target_include_directories(sauros
    PUBLIC 
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src/third_party/hwinfo/include)

target_link_libraries(sauros 
   PUBLIC hwinfo::HWinfo
   ${LIBSAUROS_LIBRARIES}
   dl
)

#
# Install bin
#
install (TARGETS sauros
    DESTINATION bin)

#
# Copy stdlib to build dir
#
#add_custom_target(copy_assets
#    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/test_scripts/importable ${CMAKE_CURRENT_BINARY_DIR}/importable
#)
#add_dependencies(sauros copy_assets)