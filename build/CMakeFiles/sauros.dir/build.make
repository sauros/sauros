# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/bosley/workspace/sauros

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/bosley/workspace/sauros/build

# Include any dependencies generated for this target.
include CMakeFiles/sauros.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/sauros.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/sauros.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/sauros.dir/flags.make

CMakeFiles/sauros.dir/app/main.cpp.o: CMakeFiles/sauros.dir/flags.make
CMakeFiles/sauros.dir/app/main.cpp.o: ../app/main.cpp
CMakeFiles/sauros.dir/app/main.cpp.o: CMakeFiles/sauros.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bosley/workspace/sauros/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/sauros.dir/app/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sauros.dir/app/main.cpp.o -MF CMakeFiles/sauros.dir/app/main.cpp.o.d -o CMakeFiles/sauros.dir/app/main.cpp.o -c /home/bosley/workspace/sauros/app/main.cpp

CMakeFiles/sauros.dir/app/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sauros.dir/app/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bosley/workspace/sauros/app/main.cpp > CMakeFiles/sauros.dir/app/main.cpp.i

CMakeFiles/sauros.dir/app/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sauros.dir/app/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bosley/workspace/sauros/app/main.cpp -o CMakeFiles/sauros.dir/app/main.cpp.s

CMakeFiles/sauros.dir/sauros/parser.cpp.o: CMakeFiles/sauros.dir/flags.make
CMakeFiles/sauros.dir/sauros/parser.cpp.o: ../sauros/parser.cpp
CMakeFiles/sauros.dir/sauros/parser.cpp.o: CMakeFiles/sauros.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bosley/workspace/sauros/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/sauros.dir/sauros/parser.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sauros.dir/sauros/parser.cpp.o -MF CMakeFiles/sauros.dir/sauros/parser.cpp.o.d -o CMakeFiles/sauros.dir/sauros/parser.cpp.o -c /home/bosley/workspace/sauros/sauros/parser.cpp

CMakeFiles/sauros.dir/sauros/parser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sauros.dir/sauros/parser.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bosley/workspace/sauros/sauros/parser.cpp > CMakeFiles/sauros.dir/sauros/parser.cpp.i

CMakeFiles/sauros.dir/sauros/parser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sauros.dir/sauros/parser.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bosley/workspace/sauros/sauros/parser.cpp -o CMakeFiles/sauros.dir/sauros/parser.cpp.s

# Object files for target sauros
sauros_OBJECTS = \
"CMakeFiles/sauros.dir/app/main.cpp.o" \
"CMakeFiles/sauros.dir/sauros/parser.cpp.o"

# External object files for target sauros
sauros_EXTERNAL_OBJECTS =

sauros: CMakeFiles/sauros.dir/app/main.cpp.o
sauros: CMakeFiles/sauros.dir/sauros/parser.cpp.o
sauros: CMakeFiles/sauros.dir/build.make
sauros: CMakeFiles/sauros.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/bosley/workspace/sauros/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable sauros"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/sauros.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/sauros.dir/build: sauros
.PHONY : CMakeFiles/sauros.dir/build

CMakeFiles/sauros.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/sauros.dir/cmake_clean.cmake
.PHONY : CMakeFiles/sauros.dir/clean

CMakeFiles/sauros.dir/depend:
	cd /home/bosley/workspace/sauros/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/bosley/workspace/sauros /home/bosley/workspace/sauros /home/bosley/workspace/sauros/build /home/bosley/workspace/sauros/build /home/bosley/workspace/sauros/build/CMakeFiles/sauros.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/sauros.dir/depend

