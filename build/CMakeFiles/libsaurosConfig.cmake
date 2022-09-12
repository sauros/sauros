# - Config file for 'libsauros' package
# It defines the following variables
#  LIBSAUROS_INCLUDE_DIRS - include directories
#  LIBSAUROS_LIBRARIES    - libraries to link against

# Include directory
set(LIBSAUROS_INCLUDE_DIRS "/usr/local/include")

# Import the exported targets
include("/usr/local/lib/cmake/libsauros/libsaurosTargets.cmake")

# Set the expected library variable
set(LIBSAUROS_LIBRARIES libsauros)
