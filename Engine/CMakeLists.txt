# CMakeList.txt : CMake project for Engine, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.12)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

set(GLAD ../Include/glad/src/glad.c)

set(SRC main.cpp main.h
        Engine.cpp Engine.h
        Components/Index.cpp Components/Index.h
        Components/Dirty.cpp Components/Dirty.h
        Components/Mesh.cpp Components/Mesh.h
        Components/GameObject.cpp Components/GameObject.h
        Systems/Debug/LiveReloadSystem.cpp Systems/Debug/LiveReloadSystem.h
        Components/ComponentBase.h Components/ComponentBase.cpp
        SystemCalls.cpp SystemCalls.h
        Systems/Physics/PhysicsSystem.cpp Systems/Physics/PhysicsSystem.h
        Systems/Rendering/RenderingSystem.cpp Systems/Rendering/RenderingSystem.h
        Systems/Rendering/RenderingBatch.h Systems/Rendering/RenderingBatch.cpp
        Systems/Pipeline/ShaderLoader.cpp Systems/Pipeline/ShaderLoader.h
        Systems/Camera/CameraSystem.cpp Systems/Camera/CameraSystem.h
        Systems/Input/InputSystem.cpp Systems/Input/InputSystem.h
        Systems/Pipeline/MeshLoader.h
        Components/Shader.cpp Components/Shader.h
        Components/ComponentId.cpp Components/ComponentId.h
        Components/Camera.cpp Components/Camera.h
        Systems/Rendering/Rendering.h
        Components/Plane.cpp Components/Plane.h
        Systems/Pipeline/EntityLoader.cpp Systems/Pipeline/EntityLoader.h
        Components/Model.cpp Components/Model.h
        Components/Vehicle.cpp Components/Vehicle.h
        Components/EventHandler.cpp Components/EventHandler.h
        Components/ComponentEvent.cpp Components/ComponentEvent.h
        Systems/Rendering/BillboardSystem.cpp Systems/Rendering/BillboardSystem.h
        Components/Billboard.cpp Components/Billboard.h
        Components/Component.h
        Systems/Events/Events.cpp Systems/Events/Events.h
        )

# Add source to this project's executable.
add_executable (Engine
        ${GLAD}
        ${SRC})

set_property(TARGET Engine PROPERTY CXX_STANDARD 17)
set_property(TARGET Engine PROPERTY CMAKE_CXX_STANDARD_REQUIRED ON)

#[[if(MSVC)
    target_compile_options(Engine PRIVATE /W4)
else()
    target_compile_options(Engine PRIVATE -Wall -Wextra -pedantic)
endif()]]

# GAME ASSETS
# copy assets into build directory

set(PHYSX_HOME PATH "")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory("../Include/glfw-3.3.1" "${CMAKE_CURRENT_BINARY_DIR}/glfw")
add_subdirectory("../Assets/" "${CMAKE_CURRENT_BINARY_DIR}/Assets")

target_link_libraries(Engine
        PUBLIC glfw
        #[[PUBLIC c++fs]]
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../libs/x64/assimp-vc141-mtd.lib"
        #[[PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../libs/x64/zlibstaticd.lib"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../libs/x64/IrrXMLd.lib"]]
        PUBLIC ${OPENGL_gl_LIBRARY})

configure_file("../dlls/assimp-vc141-mtd.dll" assimp-vc141-mtd.dll COPYONLY)
configure_file("../dlls/PhysXDevice64.dll" PhysxDevice64.dll COPYONLY)
configure_file("../dlls/glut32.dll" glut32.dll COPYONLY)
configure_file("../dlls/PhysXGpu_64.dll" PhysXGpu_64.dll COPYONLY)

include_directories("../Include/PhysX/pxshared/include")
include_directories("../Include/PhysX/include")
include_directories("../Include/include")
include_directories("../Include/glminclude")
include_directories("../Include/assimp/include")
include_directories("../Include/glad/include")

find_package(OpenGL REQUIRED)

target_include_directories(Engine PUBLIC ${OPENGL_INCLUDE_DIR})
