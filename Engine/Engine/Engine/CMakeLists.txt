# CMakeList.txt : CMake project for Engine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.13)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

set(GLAD ${CMAKE_CURRENT_SOURCE_DIR}/../Middleware/glad/src/glad.c Systems/Pipeline/ShaderLoader.cpp Systems/Pipeline/ShaderLoader.h Components/Shader.cpp Components/Shader.h Components/ComponentId.cpp Components/ComponentId.h)

# Add source to this project's executable.
add_executable (Engine "Engine.cpp" "Engine.h"
        Systems/PhysicsSystem.cpp Systems/PhysicsSystem.h Systems/Rendering/RenderingSystem.cpp Systems/Rendering/RenderingSystem.h SystemCalls.cpp SystemCalls.h
        Systems/Rendering/RenderingBatch.h Systems/Rendering/RenderingBatch.cpp
        ${GLAD}
        )

set_property(TARGET Engine PROPERTY CXX_STANDARD 17)

# GAME ASSETS
# copy glsl shader assets into build directory

FILE(GLOB SHADERS Assets/Shaders/*.vert Assets/Shaders/*.frag)

foreach(SHADER ${SHADERS})
    FILE(RELATIVE_PATH ITEM_PATH_REL ${CMAKE_CURRENT_SOURCE_DIR} ${SHADER})
    configure_file(${ITEM_PATH_REL} ${CMAKE_CURRENT_BINARY_DIR}/${ITEM_PATH_REL} COPYONLY)
endforeach()

set(PHYSX_HOME PATH "")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../Middleware/glfw-3.3.1" "${CMAKE_CURRENT_BINARY_DIR}/glfw")

target_link_libraries(Engine glfw)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/PhysxDevice64.dll" PhysxDevice64.dll COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/glut32.dll" glut32.dll COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/PhysXGpu_64.dll" PhysXGpu_64.dll COPYONLY)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../Middleware/PhysX/pxshared/include/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../Middleware/PhysX/include/")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../Middleware/glad/include")



find_package(OpenGL REQUIRED)

target_include_directories(Engine PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(Engine ${OPENGL_gl_LIBRARY})