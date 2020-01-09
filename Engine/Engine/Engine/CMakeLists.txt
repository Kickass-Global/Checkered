# CMakeList.txt : CMake project for Engine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake")

# Add source to this project's executable.
add_executable (Engine "Engine.cpp" "Engine.h")

# TODO: Add tests and install targets if needed.

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../Middleware/glfw-3.3.1" "${CMAKE_CURRENT_BINARY_DIR}/glfw")

target_link_libraries(Engine glfw)

find_package(OpenGL REQUIRED)

target_include_directories(Engine PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(Engine ${OPENGL_gl_LIBRARY})