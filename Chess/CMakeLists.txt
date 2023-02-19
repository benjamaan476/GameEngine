# CMakeList.txt : CMake project for Chess, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(spdlog REQUIRED)

include_directories(${GLM_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIRS})

add_compile_definitions(_PROJECT_DIR_="${CMAKE_CURRENT_SOURCE_DIR}")

# Add source to this project's executable.
add_executable (Chess "main.cpp" "../GameEngine/Log/Log.cpp" "Pieces/Piece.h" "Pieces/Move.h" "ChessCore.h" "Board.cpp" "Bitboard.h" "Bitboard.cpp" "MoveSet.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Chess PROPERTY CXX_STANDARD 20)
endif()

target_compile_options(Chess PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W3 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

target_link_libraries(Chess PRIVATE glm::glm glfw spdlog::spdlog)