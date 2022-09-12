#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libsauros" for configuration "Debug"
set_property(TARGET libsauros APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(libsauros PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "/usr/local/lib/libsauros.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS libsauros )
list(APPEND _IMPORT_CHECK_FILES_FOR_libsauros "/usr/local/lib/libsauros.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
