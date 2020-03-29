# CMakeList.txt : CMake project for Engine, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.12)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

set(GLAD ../Include/glad/src/glad.c)

file(GLOB_RECURSE PHYSX_SNIPPETS ../Engine/Systems/Physics/snippet** *.h *.cpp)

set(SRC main.cpp main.h
	Scenarios/testworld.cpp Scenarios/testworld.hpp
        Systems/Engine.cpp Systems/Engine.h
        Components/tags.cpp Components/tags.h
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
        Systems/Physics/SimulationCallback.h Systems/Physics/SimulationCallback.cpp
        Components/Shader.cpp Components/Shader.h
        Components/ComponentId.cpp Components/ComponentId.h
        Components/ComponentId.tpp.h
        Components/Camera.cpp Components/Camera.h
        Systems/Rendering/Rendering.h
        Systems/Physics/FilterShader.cpp Systems/Physics/FilterShader.h
        Components/Plane.cpp Components/Plane.h
        Systems/Pipeline/EntityLoader.cpp Systems/Pipeline/EntityLoader.h
        Components/Model.cpp Components/Model.h
        Components/ReportCard.cpp Components/ReportCard.h
        Components/Passenger.cpp Components/Passenger.h
        Components/Vehicle.cpp Components/Vehicle.h
        Components/EventHandler.cpp Components/EventHandler.h
        Components/EventDelegate.cpp Components/EventDelegate.h
        Systems/Rendering/BillboardSystem.cpp Systems/Rendering/BillboardSystem.h
        Components/Billboard.cpp Components/Billboard.h
        Components/Component.h
        Components/Sound.h Components/Sound.cpp
        Systems/Events/Events.cpp Systems/Events/Events.h
        Systems/Pipeline/Library.cpp Systems/Pipeline/Library.h
        ${PHYSX_SNIPPETS}
        Systems/Pipeline/pipeline.cpp
        Systems/Pipeline/pipeline.hpp
        Systems/Damage/damagesystem.cpp
        Systems/Damage/damagesystem.hpp
        Components/damage.cpp Components/damage.hpp
        Systems/SystemInterface.hpp Systems/Component/scenecomponentsystem.cpp
        Systems/Component/scenecomponentsystem.hpp Components/physicspacket.cpp
        Components/physicspacket.hpp
        Systems/hornSystem/hornSystem.cpp
        Systems/hornSystem/hornSystem.h
        Systems/Vehicle/vehiclesystem.cpp Systems/Vehicle/vehiclesystem.hpp
        Systems/soundSystem/SoundSystem.cpp Systems/soundSystem/SoundSystem.h
        Components/WorldTransform.h Components/WorldTransform.cpp
        Components/texture.cpp Components/texture.hpp
        Components/material.cpp Components/material.hpp 
        Components/PhysicsActor.cpp Components/PhysicsActor.h 
        Systems/Component/scenerysystem.cpp Systems/Component/scenerysystem.h
        Systems/EngineStore.cpp Systems/EngineStore.h
        Systems/Physics/VehicleFactory.cpp Systems/Physics/VehicleFactory.hpp 
        Systems/Physics/VehicleFactoryFunctions.cpp Systems/Physics/VehicleFactoryFunctions.hpp 
        Components/TransformData.cpp Components/TransformData.hpp 
        Components/bitmap.cpp Components/bitmap.hpp 
        Systems/Rendering/fontsystem.cpp Systems/Rendering/fontsystem.hpp 
        Systems/Rendering/text.cpp Systems/Rendering/text.hpp 
        Scenarios/start.cpp Scenarios/start.hpp 
        Scenarios/scenariointerface.cpp Scenarios/scenariointerface.hpp 
        Systems/EngineDebug.cpp Systems/EngineDebug.hpp Components/Node.cpp Components/Node.hpp)

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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSVC_COMPILE_FLAGS}")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory("../Include/glfw-3.3.1" "${CMAKE_CURRENT_BINARY_DIR}/glfw")

target_link_libraries(Engine
        PRIVATE glfw
        PRIVATE SOIL
        PRIVATE freetype

        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../3rd-party/assimp/x64-Debug/lib/assimp-vc142-mtd.lib
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../3rd-party/assimp/x64-Debug/lib/IrrXMLd.lib
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../3rd-party/assimp/x64-Debug/lib/zlibstaticd.lib

        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../3rd-party/OpenAL/libs/Win64/OpenAL32.lib
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
        PRIVATE "Scenarios"
        SYSTEM PRIVATE "../3rd-party/OpenAL/include"
        SYSTEM PRIVATE "../Include/PhysX/include"
        SYSTEM PRIVATE "../Include/PhysX/source/foundation/include"
        SYSTEM PRIVATE "../Include/PxShared/include"
        SYSTEM PRIVATE "../Include/include"
        SYSTEM PRIVATE "../Include/glminclude"
        SYSTEM PRIVATE ../3rd-party/assimp/x64-Debug/include
        SYSTEM PRIVATE "../Include/glad/include")

add_custom_command(
        TARGET Engine POST_BUILD
        COMMENT "Copying assets to engine directory"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Assets
        $<TARGET_FILE_DIR:Engine>/Assets)

