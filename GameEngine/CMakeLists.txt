# CMakeList.txt : CMake project for GameEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.16)

find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Vulkan REQUIRED COMPONENTS glslc)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)
find_package(spdlog REQUIRED)
find_package(imgui CONFIG REQUIRED)

include_directories(${GLM_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${Vulkan_INCLUDE_DIRS})
include_directories(${Imgui_INCLUDE_DIRS})

add_compile_definitions(_PROJECT_DIR_="${CMAKE_CURRENT_SOURCE_DIR}")

# Add source to this project's executable.
add_executable (GameEngine "main.cpp" "Application.cpp" "Log/Log.cpp" "Platform/Window.cpp" "Platform/Windows/WindowsWindow.cpp" "renderer/Texture.cpp" "renderer/buffer.cpp" "renderer/CommandBuffer.cpp" "ui/gui.cpp" "renderer/Renderer.cpp" "renderer/Sprite.cpp" "renderer/SpriteRenderer.cpp" "renderer/Pipeline.cpp" "ApplicationCore.cpp" "renderer/Initialisers.h" "Instrumentor.h" "Instrumentor.cpp" "renderer/Initialisers.cpp" "renderer/RendererCore.cpp" "renderer/Model.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GameEngine PROPERTY CXX_STANDARD 20)
endif()

target_compile_options(GameEngine PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

target_link_libraries(GameEngine PRIVATE glm::glm glfw ${Vulkan_LIBRARIES} spdlog::spdlog imgui::imgui)