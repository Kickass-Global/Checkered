# CMakeList.txt : CMake project for Engine, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.12)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

set(GLAD ../Include/glad/src/glad.c)

file(GLOB_RECURSE PHYSX_SNIPPETS ../Engine/Systems/Physics/snippet** *.h *.cpp)

set(SRC main.cpp main.h
        Systems/Engine.cpp Systems/Engine.h
        Components/Index.cpp Components/Index.h
        Components/Dirty.cpp Components/Dirty.h
        Components/Mesh.cpp Components/Mesh.h
        Components/SceneComponent.cpp Components/SceneComponent.h
        Systems/Debug/LiveReloadSystem.cpp Systems/Debug/LiveReloadSystem.h
        Components/ComponentBase.h Components/ComponentBase.cpp
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
        Systems/Pipeline/Library.cpp Systems/Pipeline/Library.h
        ${PHYSX_SNIPPETS}
        Systems/Pipeline/pipeline.cpp
        Systems/Pipeline/pipeline.hpp
        Systems/Damage/damagesystem.cpp
        Systems/Damage/damagesystem.hpp
        Components/damage.cpp Components/damage.hpp
        Systems/systeminterface.hpp Systems/Component/scenecomponentsystem.cpp
        Systems/Component/scenecomponentsystem.hpp Components/physicshandler.cpp
        Components/physicshandler.hpp
        Systems/soundSystem/sound.cpp
        Systems/soundSystem/sound.h
        Systems/hornSystem/hornSystem.cpp
        Systems/hornSystem/hornSystem.h
        )

# Add source to this project's executable.
add_executable(Engine
        ${GLAD}
        ${SRC})

set_property(TARGET Engine PROPERTY CXX_STANDARD 17)
set_property(TARGET Engine PROPERTY CMAKE_CXX_STANDARD_REQUIRED ON)

macro(remove_flag_from_target _target _flag)
    get_target_property(_target_cxx_flags ${_target} COMPILE_OPTIONS)
    if (_target_cxx_flags)
        list(REMOVE_ITEM _target_cxx_flags ${_flag})
        set_target_properties(${_target} PROPERTIES COMPILE_OPTIONS "${_target_cxx_flags}")
    endif ()
endmacro()


if (MSVC)
    remove_flag_from_target(Engine /W3)
    target_compile_options(Engine PRIVATE /W3)
else ()
    target_compile_options(Engine PRIVATE -Wall -Wextra -pedantic)
endif ()


# GAME ASSETS
# copy assets into build directory

set(PHYSX_HOME PATH "")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory("../Include/glfw-3.3.1" "${CMAKE_CURRENT_BINARY_DIR}/glfw")
add_subdirectory("../Assets/" "${CMAKE_CURRENT_BINARY_DIR}/Assets")

target_link_libraries(Engine
        PRIVATE glfw
        #[[PUBLIC c++fs]]
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../libs/x64/assimp-vc141-mtd.lib"
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../libs/x64/PhysX_64.lib
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../libs/x64/PhysXCharacterKinematic_static_64.lib
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../libs/x64/PhysXCommon_64.lib
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../libs/x64/PhysXCooking_64.lib
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../libs/x64/PhysXExtensions_static_64.lib
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../libs/x64/PhysXFoundation_64.lib
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../libs/x64/PhysXPvdSDK_static_64.lib
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../libs/x64/PhysXTask_static_64.lib
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../libs/x64/PhysXVehicle_static_64.lib
        PRIVATE ${OPENGL_gl_LIBRARY})

configure_file("../dlls/assimp-vc141-mtd.dll" assimp-vc141-mtd.dll COPYONLY)
configure_file("../dlls/PhysXGpu_64.dll" PhysXGpu_64.dll COPYONLY)
configure_file("../dlls/PhysX_64.dll" PhysX_64.dll COPYONLY)
configure_file("../dlls/PhysXCommon_64.dll" PhysXCommon_64.dll COPYONLY)
configure_file("../dlls/PhysXCooking_64.dll" PhysXCooking_64.dll COPYONLY)
configure_file("../dlls/PhysXDevice64.dll" PhysXDevice64.dll COPYONLY)
configure_file("../dlls/PhysXFoundation_64.dll" PhysXFoundation_64.dll COPYONLY)
configure_file("../dlls/glut32.dll" glut32.dll COPYONLY)

find_package(OpenGL REQUIRED)

target_include_directories(Engine SYSTEM PRIVATE ${OPENGL_INCLUDE_DIR}
        PRIVATE "Components"
        PRIVATE "Systems"
        SYSTEM PRIVATE "../Include/PhysX/include"
        SYSTEM PRIVATE "../Include/PhysX/source/foundation/include"
        SYSTEM PRIVATE "../Include/PxShared/include"
        SYSTEM PRIVATE "../Include/include"
        SYSTEM PRIVATE "../Include/glminclude"
        SYSTEM PRIVATE "../Include/assimp/include"
        SYSTEM PRIVATE "../Include/glad/include")
