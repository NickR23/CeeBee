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
CMAKE_SOURCE_DIR = /home/nr/code/CeeBee/cmocka-1.1.2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/nr/code/CeeBee/cmocka-build

# Include any dependencies generated for this target.
include example/CMakeFiles/allocate_module_test.dir/depend.make

# Include the progress variables for this target.
include example/CMakeFiles/allocate_module_test.dir/progress.make

# Include the compile flags for this target's objects.
include example/CMakeFiles/allocate_module_test.dir/flags.make

example/CMakeFiles/allocate_module_test.dir/allocate_module.c.o: example/CMakeFiles/allocate_module_test.dir/flags.make
example/CMakeFiles/allocate_module_test.dir/allocate_module.c.o: /home/nr/code/CeeBee/cmocka-1.1.2/example/allocate_module.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nr/code/CeeBee/cmocka-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object example/CMakeFiles/allocate_module_test.dir/allocate_module.c.o"
	cd /home/nr/code/CeeBee/cmocka-build/example && /usr/bin/cc $(C_DEFINES) -DUNIT_TESTING=1 $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/allocate_module_test.dir/allocate_module.c.o   -c /home/nr/code/CeeBee/cmocka-1.1.2/example/allocate_module.c

example/CMakeFiles/allocate_module_test.dir/allocate_module.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/allocate_module_test.dir/allocate_module.c.i"
	cd /home/nr/code/CeeBee/cmocka-build/example && /usr/bin/cc $(C_DEFINES) -DUNIT_TESTING=1 $(C_INCLUDES) $(C_FLAGS) -E /home/nr/code/CeeBee/cmocka-1.1.2/example/allocate_module.c > CMakeFiles/allocate_module_test.dir/allocate_module.c.i

example/CMakeFiles/allocate_module_test.dir/allocate_module.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/allocate_module_test.dir/allocate_module.c.s"
	cd /home/nr/code/CeeBee/cmocka-build/example && /usr/bin/cc $(C_DEFINES) -DUNIT_TESTING=1 $(C_INCLUDES) $(C_FLAGS) -S /home/nr/code/CeeBee/cmocka-1.1.2/example/allocate_module.c -o CMakeFiles/allocate_module_test.dir/allocate_module.c.s

example/CMakeFiles/allocate_module_test.dir/allocate_module_test.c.o: example/CMakeFiles/allocate_module_test.dir/flags.make
example/CMakeFiles/allocate_module_test.dir/allocate_module_test.c.o: /home/nr/code/CeeBee/cmocka-1.1.2/example/allocate_module_test.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/nr/code/CeeBee/cmocka-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object example/CMakeFiles/allocate_module_test.dir/allocate_module_test.c.o"
	cd /home/nr/code/CeeBee/cmocka-build/example && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/allocate_module_test.dir/allocate_module_test.c.o   -c /home/nr/code/CeeBee/cmocka-1.1.2/example/allocate_module_test.c

example/CMakeFiles/allocate_module_test.dir/allocate_module_test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/allocate_module_test.dir/allocate_module_test.c.i"
	cd /home/nr/code/CeeBee/cmocka-build/example && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/nr/code/CeeBee/cmocka-1.1.2/example/allocate_module_test.c > CMakeFiles/allocate_module_test.dir/allocate_module_test.c.i

example/CMakeFiles/allocate_module_test.dir/allocate_module_test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/allocate_module_test.dir/allocate_module_test.c.s"
	cd /home/nr/code/CeeBee/cmocka-build/example && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/nr/code/CeeBee/cmocka-1.1.2/example/allocate_module_test.c -o CMakeFiles/allocate_module_test.dir/allocate_module_test.c.s

# Object files for target allocate_module_test
allocate_module_test_OBJECTS = \
"CMakeFiles/allocate_module_test.dir/allocate_module.c.o" \
"CMakeFiles/allocate_module_test.dir/allocate_module_test.c.o"

# External object files for target allocate_module_test
allocate_module_test_EXTERNAL_OBJECTS =

example/allocate_module_test: example/CMakeFiles/allocate_module_test.dir/allocate_module.c.o
example/allocate_module_test: example/CMakeFiles/allocate_module_test.dir/allocate_module_test.c.o
example/allocate_module_test: example/CMakeFiles/allocate_module_test.dir/build.make
example/allocate_module_test: src/libcmocka.so.0.5.0
example/allocate_module_test: example/CMakeFiles/allocate_module_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/nr/code/CeeBee/cmocka-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable allocate_module_test"
	cd /home/nr/code/CeeBee/cmocka-build/example && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/allocate_module_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
example/CMakeFiles/allocate_module_test.dir/build: example/allocate_module_test

.PHONY : example/CMakeFiles/allocate_module_test.dir/build

example/CMakeFiles/allocate_module_test.dir/clean:
	cd /home/nr/code/CeeBee/cmocka-build/example && $(CMAKE_COMMAND) -P CMakeFiles/allocate_module_test.dir/cmake_clean.cmake
.PHONY : example/CMakeFiles/allocate_module_test.dir/clean

example/CMakeFiles/allocate_module_test.dir/depend:
	cd /home/nr/code/CeeBee/cmocka-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/nr/code/CeeBee/cmocka-1.1.2 /home/nr/code/CeeBee/cmocka-1.1.2/example /home/nr/code/CeeBee/cmocka-build /home/nr/code/CeeBee/cmocka-build/example /home/nr/code/CeeBee/cmocka-build/example/CMakeFiles/allocate_module_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : example/CMakeFiles/allocate_module_test.dir/depend

