# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("MineSweeperCpp")

# Include sub-projects.
add_subdirectory ("MineSweeperCpp")

set(SOURCE_FILES main.cpp)

add_executable(simple_example ${SOURCE_FILES})
