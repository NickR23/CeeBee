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
CMAKE_SOURCE_DIR = /home/nr/code/CeeBee

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/nr/code/CeeBee

# Include any dependencies generated for this target.
include test/CMakeFiles/operations_tests.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/operations_tests.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/operations_tests.dir/flags.make

test/CMakeFiles/operations_tests.dir/__/src/common.c.o: test/CMakeFiles/operations_tests.dir/flags.make
test/CMakeFiles/operations_tests.dir/__/src/common.c.o: src/common.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nr/code/CeeBee/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object test/CMakeFiles/operations_tests.dir/__/src/common.c.o"
	cd /home/nr/code/CeeBee/test && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/operations_tests.dir/__/src/common.c.o   -c /home/nr/code/CeeBee/src/common.c

test/CMakeFiles/operations_tests.dir/__/src/common.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/operations_tests.dir/__/src/common.c.i"
	cd /home/nr/code/CeeBee/test && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/nr/code/CeeBee/src/common.c > CMakeFiles/operations_tests.dir/__/src/common.c.i

test/CMakeFiles/operations_tests.dir/__/src/common.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/operations_tests.dir/__/src/common.c.s"
	cd /home/nr/code/CeeBee/test && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/nr/code/CeeBee/src/common.c -o CMakeFiles/operations_tests.dir/__/src/common.c.s

test/CMakeFiles/operations_tests.dir/__/src/cpu.c.o: test/CMakeFiles/operations_tests.dir/flags.make
test/CMakeFiles/operations_tests.dir/__/src/cpu.c.o: src/cpu.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nr/code/CeeBee/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object test/CMakeFiles/operations_tests.dir/__/src/cpu.c.o"
	cd /home/nr/code/CeeBee/test && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/operations_tests.dir/__/src/cpu.c.o   -c /home/nr/code/CeeBee/src/cpu.c

test/CMakeFiles/operations_tests.dir/__/src/cpu.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/operations_tests.dir/__/src/cpu.c.i"
	cd /home/nr/code/CeeBee/test && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/nr/code/CeeBee/src/cpu.c > CMakeFiles/operations_tests.dir/__/src/cpu.c.i

test/CMakeFiles/operations_tests.dir/__/src/cpu.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/operations_tests.dir/__/src/cpu.c.s"
	cd /home/nr/code/CeeBee/test && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/nr/code/CeeBee/src/cpu.c -o CMakeFiles/operations_tests.dir/__/src/cpu.c.s

test/CMakeFiles/operations_tests.dir/__/src/operations.c.o: test/CMakeFiles/operations_tests.dir/flags.make
test/CMakeFiles/operations_tests.dir/__/src/operations.c.o: src/operations.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nr/code/CeeBee/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object test/CMakeFiles/operations_tests.dir/__/src/operations.c.o"
	cd /home/nr/code/CeeBee/test && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/operations_tests.dir/__/src/operations.c.o   -c /home/nr/code/CeeBee/src/operations.c

test/CMakeFiles/operations_tests.dir/__/src/operations.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/operations_tests.dir/__/src/operations.c.i"
	cd /home/nr/code/CeeBee/test && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/nr/code/CeeBee/src/operations.c > CMakeFiles/operations_tests.dir/__/src/operations.c.i

test/CMakeFiles/operations_tests.dir/__/src/operations.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/operations_tests.dir/__/src/operations.c.s"
	cd /home/nr/code/CeeBee/test && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/nr/code/CeeBee/src/operations.c -o CMakeFiles/operations_tests.dir/__/src/operations.c.s

test/CMakeFiles/operations_tests.dir/operations.cpp.o: test/CMakeFiles/operations_tests.dir/flags.make
test/CMakeFiles/operations_tests.dir/operations.cpp.o: test/operations.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nr/code/CeeBee/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object test/CMakeFiles/operations_tests.dir/operations.cpp.o"
	cd /home/nr/code/CeeBee/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/operations_tests.dir/operations.cpp.o -c /home/nr/code/CeeBee/test/operations.cpp

test/CMakeFiles/operations_tests.dir/operations.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/operations_tests.dir/operations.cpp.i"
	cd /home/nr/code/CeeBee/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nr/code/CeeBee/test/operations.cpp > CMakeFiles/operations_tests.dir/operations.cpp.i

test/CMakeFiles/operations_tests.dir/operations.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/operations_tests.dir/operations.cpp.s"
	cd /home/nr/code/CeeBee/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nr/code/CeeBee/test/operations.cpp -o CMakeFiles/operations_tests.dir/operations.cpp.s

test/CMakeFiles/operations_tests.dir/main.cpp.o: test/CMakeFiles/operations_tests.dir/flags.make
test/CMakeFiles/operations_tests.dir/main.cpp.o: test/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nr/code/CeeBee/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object test/CMakeFiles/operations_tests.dir/main.cpp.o"
	cd /home/nr/code/CeeBee/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/operations_tests.dir/main.cpp.o -c /home/nr/code/CeeBee/test/main.cpp

test/CMakeFiles/operations_tests.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/operations_tests.dir/main.cpp.i"
	cd /home/nr/code/CeeBee/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/nr/code/CeeBee/test/main.cpp > CMakeFiles/operations_tests.dir/main.cpp.i

test/CMakeFiles/operations_tests.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/operations_tests.dir/main.cpp.s"
	cd /home/nr/code/CeeBee/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/nr/code/CeeBee/test/main.cpp -o CMakeFiles/operations_tests.dir/main.cpp.s

# Object files for target operations_tests
operations_tests_OBJECTS = \
"CMakeFiles/operations_tests.dir/__/src/common.c.o" \
"CMakeFiles/operations_tests.dir/__/src/cpu.c.o" \
"CMakeFiles/operations_tests.dir/__/src/operations.c.o" \
"CMakeFiles/operations_tests.dir/operations.cpp.o" \
"CMakeFiles/operations_tests.dir/main.cpp.o"

# External object files for target operations_tests
operations_tests_EXTERNAL_OBJECTS =

test/operations_tests: test/CMakeFiles/operations_tests.dir/__/src/common.c.o
test/operations_tests: test/CMakeFiles/operations_tests.dir/__/src/cpu.c.o
test/operations_tests: test/CMakeFiles/operations_tests.dir/__/src/operations.c.o
test/operations_tests: test/CMakeFiles/operations_tests.dir/operations.cpp.o
test/operations_tests: test/CMakeFiles/operations_tests.dir/main.cpp.o
test/operations_tests: test/CMakeFiles/operations_tests.dir/build.make
test/operations_tests: googletest-build/googlemock/gtest/libgtest_main.a
test/operations_tests: googletest-build/googlemock/gtest/libgtest.a
test/operations_tests: test/CMakeFiles/operations_tests.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/nr/code/CeeBee/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX executable operations_tests"
	cd /home/nr/code/CeeBee/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/operations_tests.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/operations_tests.dir/build: test/operations_tests

.PHONY : test/CMakeFiles/operations_tests.dir/build

test/CMakeFiles/operations_tests.dir/clean:
	cd /home/nr/code/CeeBee/test && $(CMAKE_COMMAND) -P CMakeFiles/operations_tests.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/operations_tests.dir/clean

test/CMakeFiles/operations_tests.dir/depend:
	cd /home/nr/code/CeeBee && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/nr/code/CeeBee /home/nr/code/CeeBee/test /home/nr/code/CeeBee /home/nr/code/CeeBee/test /home/nr/code/CeeBee/test/CMakeFiles/operations_tests.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/operations_tests.dir/depend
