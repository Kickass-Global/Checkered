// Engine.cpp : Defines the entry point for the application.
//

#include <functional>

#include "Systems/Rendering/RenderingSystem.h"
#include "Systems/Pipeline/ShaderLoader.h"
#include "Systems/Rendering/RenderingBatch.h"
#include "Systems/Input/InputSystem.h"
#include "Systems/Camera/CameraSystem.h"
#include "Systems/Pipeline/MeshLoader.h"

using Type = Component::Index::Type;

int main() {

    using namespace Engine;
    
    auto running = true;

    Rendering::RenderingSystem renderingSystem;
    renderingSystem.initialize();

    // hookup inputs from current window
    Input::InputSystem::initialize(renderingSystem.getWindow());

    // hookup key press event with camera system
    Camera::CameraSystem cameraSystem;

    Input::InputSystem::onKeyPress += std::bind(&Camera::CameraSystem::onKeyPress,
            &cameraSystem, std::placeholders::_1);

    Input::InputSystem::onKeyDown += std::bind(&Camera::CameraSystem::onKeyDown,
        &cameraSystem, std::placeholders::_1);

    Input::InputSystem::onKeyUp += std::bind(&Camera::CameraSystem::onKeyUp,
        &cameraSystem, std::placeholders::_1);

    renderingSystem.onWindowSizeChanged += std::bind(&Camera::CameraSystem::onWindowSizeChanged,
        &cameraSystem, std::placeholders::_1, std::placeholders::_2);

    // Component to reference the shader
    Component::Shader shader;

    // simulate content pipeline loading
    Pipeline::ProgramLoader loader;

    // load a simple shader program
    renderingSystem.push_back(
            shader.id(),
            loader.load("Assets/Programs/basic.shader")
    );
    

    Rendering::RenderBatch batch(1000);

    float vertex_data[] = {
            0.9f, -0.9f, 0,
            -0.9f, -0.9f, 0,
            0, 0.9f, 0
    };

    // push batch data and assign shader program to batch
    batch.push_back(vertex_data, sizeof(vertex_data));
    batch.assign_shader(shader);
    
    renderingSystem.push_back(batch);

    // make a default camera
    auto camera = std::make_shared<Component::Camera>();
    Component::Index::push_entity(Type::Camera, camera->id(), camera);

    // setup a game clock
    std::chrono::high_resolution_clock clock;
   
    auto start = clock.now();
    auto end = start;

    //system_calls::loggingLevel = system_calls::Importance::low;

    while (running) {

        // frametime is a measure of how 'on-time' a frame is... <1 early, >1 late.
        frametime elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 16.6666667f;
        log<module, Importance::low>("Frametime: ", elapsed);

        cameraSystem.update(elapsed);
        renderingSystem.update(elapsed);
        
        start = end;
        end = clock.now();
    }
}
