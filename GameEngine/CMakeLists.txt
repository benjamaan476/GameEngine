# CMakeList.txt : CMake project for GameEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(spdlog REQUIRED)

include_directories(${GLM_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${Vulkan_INCLUDE_DIRS})

file(GLOB_RECURSE HEADER_FILES ${PROJECT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)

add_compile_definitions(_PROJECT_DIR_="${PROJECT_SOURCE_DIR}")

# Add source to this project's executable.
add_executable (GameEngine "main.cpp" "Application.cpp" "Application.h" "Log/Log.h" "Log/Log.cpp" "Platform/Window.h" "Platform/Window.cpp" "Platform/Windows/WindowsWindow.h" "Platform/Windows/WindowsWindow.cpp" "EngineCore.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GameEngine PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(GameEngine PRIVATE glm::glm glfw ${Vulkan_LIBRARIES} spdlog::spdlog)


# TODO: Add tests and install targets if needed.
