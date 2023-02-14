# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.12)

project ("GameEngine")
project ("Chess")
project ("UnitTests")
# Include sub-projects.
add_subdirectory ("GameEngine")
add_subdirectory ("Chess")
add_subdirectory ("UnitTests")