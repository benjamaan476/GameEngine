cmake_minimum_required (VERSION 3.12)

project(UnitTests)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(runUnitTests tests.cpp "../Chess/Bitboard.cpp")

target_link_libraries(runUnitTests ${GTEST_BOTH_LIBRARIES})
add_test(runUnitTests runUnitTests)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET runUnitTests PROPERTY CXX_STANDARD 20)
endif()

