# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/mnt/c/Users/23776/Desktop/我/ThermotionXWY/SimulatorMethods/SIMULATOR FVM_method/code-topublish"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/mnt/c/Users/23776/Desktop/我/ThermotionXWY/SimulatorMethods/SIMULATOR FVM_method/code-topublish/build"

# Include any dependencies generated for this target.
include CMakeFiles/SIMULATOR.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/SIMULATOR.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/SIMULATOR.dir/flags.make

CMakeFiles/SIMULATOR.dir/src/main.cpp.o: CMakeFiles/SIMULATOR.dir/flags.make
CMakeFiles/SIMULATOR.dir/src/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/mnt/c/Users/23776/Desktop/我/ThermotionXWY/SimulatorMethods/SIMULATOR FVM_method/code-topublish/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/SIMULATOR.dir/src/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SIMULATOR.dir/src/main.cpp.o -c "/mnt/c/Users/23776/Desktop/我/ThermotionXWY/SimulatorMethods/SIMULATOR FVM_method/code-topublish/src/main.cpp"

CMakeFiles/SIMULATOR.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SIMULATOR.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/mnt/c/Users/23776/Desktop/我/ThermotionXWY/SimulatorMethods/SIMULATOR FVM_method/code-topublish/src/main.cpp" > CMakeFiles/SIMULATOR.dir/src/main.cpp.i

CMakeFiles/SIMULATOR.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SIMULATOR.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/mnt/c/Users/23776/Desktop/我/ThermotionXWY/SimulatorMethods/SIMULATOR FVM_method/code-topublish/src/main.cpp" -o CMakeFiles/SIMULATOR.dir/src/main.cpp.s

# Object files for target SIMULATOR
SIMULATOR_OBJECTS = \
"CMakeFiles/SIMULATOR.dir/src/main.cpp.o"

# External object files for target SIMULATOR
SIMULATOR_EXTERNAL_OBJECTS =

../bin/SIMULATOR: CMakeFiles/SIMULATOR.dir/src/main.cpp.o
../bin/SIMULATOR: CMakeFiles/SIMULATOR.dir/build.make
../bin/SIMULATOR: deps/vecmath/libvecmath.a
../bin/SIMULATOR: CMakeFiles/SIMULATOR.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/mnt/c/Users/23776/Desktop/我/ThermotionXWY/SimulatorMethods/SIMULATOR FVM_method/code-topublish/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/SIMULATOR"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SIMULATOR.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/SIMULATOR.dir/build: ../bin/SIMULATOR

.PHONY : CMakeFiles/SIMULATOR.dir/build

CMakeFiles/SIMULATOR.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/SIMULATOR.dir/cmake_clean.cmake
.PHONY : CMakeFiles/SIMULATOR.dir/clean

CMakeFiles/SIMULATOR.dir/depend:
	cd "/mnt/c/Users/23776/Desktop/我/ThermotionXWY/SimulatorMethods/SIMULATOR FVM_method/code-topublish/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/mnt/c/Users/23776/Desktop/我/ThermotionXWY/SimulatorMethods/SIMULATOR FVM_method/code-topublish" "/mnt/c/Users/23776/Desktop/我/ThermotionXWY/SimulatorMethods/SIMULATOR FVM_method/code-topublish" "/mnt/c/Users/23776/Desktop/我/ThermotionXWY/SimulatorMethods/SIMULATOR FVM_method/code-topublish/build" "/mnt/c/Users/23776/Desktop/我/ThermotionXWY/SimulatorMethods/SIMULATOR FVM_method/code-topublish/build" "/mnt/c/Users/23776/Desktop/我/ThermotionXWY/SimulatorMethods/SIMULATOR FVM_method/code-topublish/build/CMakeFiles/SIMULATOR.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/SIMULATOR.dir/depend

