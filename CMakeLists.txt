cmake_minimum_required(VERSION 3.1)

project(ceebee)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

##
### Test Definitions ###
##

# Download and build gtest
configure_file(CMakeLists.txt.in
        googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
        ${CMAKE_BINARY_DIR}/googletest-build)

enable_testing()
add_subdirectory(test)

##
### Source Definitions ###
##

# Tells cmake where all the headers are
include_directories("${PROJECT_SOURCE_DIR}/include")

# Get all of my .c files
file(GLOB sources "${PROJECT_SOURCE_DIR}/src/*.c")

add_executable(ceebee ${sources})
set_property(TARGET ceebee PROPERTY C_STANDARD 99)
set(CMAKE_C_FLAGS "-std=c99")
