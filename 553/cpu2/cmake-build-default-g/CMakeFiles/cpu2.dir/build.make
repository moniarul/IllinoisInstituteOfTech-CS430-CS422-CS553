# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/diesel/Desktop/Assignments&Coursework/553/cpu2"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/diesel/Desktop/Assignments&Coursework/553/cpu2/cmake-build-default-g"

# Include any dependencies generated for this target.
include CMakeFiles/cpu2.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/cpu2.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/cpu2.dir/flags.make

CMakeFiles/cpu2.dir/main.cpp.o: CMakeFiles/cpu2.dir/flags.make
CMakeFiles/cpu2.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/diesel/Desktop/Assignments&Coursework/553/cpu2/cmake-build-default-g/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/cpu2.dir/main.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/cpu2.dir/main.cpp.o -c "/Users/diesel/Desktop/Assignments&Coursework/553/cpu2/main.cpp"

CMakeFiles/cpu2.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cpu2.dir/main.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/diesel/Desktop/Assignments&Coursework/553/cpu2/main.cpp" > CMakeFiles/cpu2.dir/main.cpp.i

CMakeFiles/cpu2.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cpu2.dir/main.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/diesel/Desktop/Assignments&Coursework/553/cpu2/main.cpp" -o CMakeFiles/cpu2.dir/main.cpp.s

CMakeFiles/cpu2.dir/main.cpp.o.requires:

.PHONY : CMakeFiles/cpu2.dir/main.cpp.o.requires

CMakeFiles/cpu2.dir/main.cpp.o.provides: CMakeFiles/cpu2.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/cpu2.dir/build.make CMakeFiles/cpu2.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/cpu2.dir/main.cpp.o.provides

CMakeFiles/cpu2.dir/main.cpp.o.provides.build: CMakeFiles/cpu2.dir/main.cpp.o


# Object files for target cpu2
cpu2_OBJECTS = \
"CMakeFiles/cpu2.dir/main.cpp.o"

# External object files for target cpu2
cpu2_EXTERNAL_OBJECTS =

cpu2: CMakeFiles/cpu2.dir/main.cpp.o
cpu2: CMakeFiles/cpu2.dir/build.make
cpu2: CMakeFiles/cpu2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/Users/diesel/Desktop/Assignments&Coursework/553/cpu2/cmake-build-default-g/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable cpu2"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cpu2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/cpu2.dir/build: cpu2

.PHONY : CMakeFiles/cpu2.dir/build

CMakeFiles/cpu2.dir/requires: CMakeFiles/cpu2.dir/main.cpp.o.requires

.PHONY : CMakeFiles/cpu2.dir/requires

CMakeFiles/cpu2.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/cpu2.dir/cmake_clean.cmake
.PHONY : CMakeFiles/cpu2.dir/clean

CMakeFiles/cpu2.dir/depend:
	cd "/Users/diesel/Desktop/Assignments&Coursework/553/cpu2/cmake-build-default-g" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/diesel/Desktop/Assignments&Coursework/553/cpu2" "/Users/diesel/Desktop/Assignments&Coursework/553/cpu2" "/Users/diesel/Desktop/Assignments&Coursework/553/cpu2/cmake-build-default-g" "/Users/diesel/Desktop/Assignments&Coursework/553/cpu2/cmake-build-default-g" "/Users/diesel/Desktop/Assignments&Coursework/553/cpu2/cmake-build-default-g/CMakeFiles/cpu2.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/cpu2.dir/depend

