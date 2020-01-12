// Engine.cpp : Defines the entry point for the application.
//

#include <functional>

#include "Systems/Rendering/RenderingSystem.h"
#include "Systems/Pipeline/ShaderLoader.h"
#include "Systems/Rendering/RenderingBatch.h"
#include "Systems/Input/InputSystem.h"
#include "Systems/Camera/CameraSystem.h"

using Type = Component::Index::Type;

int main() {

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

    // simulate content pipeline loading
    Pipeline::ProgramLoader loader;

    // Component to reference the shader
    Component::Shader shader;

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

    // make a default camera
    std::shared_ptr<Component::Camera> camera = std::make_shared<Component::Camera>();
    Component::Index::push_entity(Type::Camera, camera->id(), camera);

    renderingSystem.push_back(batch);

    std::chrono::high_resolution_clock clock;
    
    auto start = clock.now();
    auto end = start;

    //system_calls::loggingLevel = system_calls::Importance::low;

    while (running) {

        frametime elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 16.6666667f;

        system_calls::log<module, system_calls::Importance::low>("Frametime: ", elapsed);

        // when a camera is dirty we need a way to let the rendering system know...
        cameraSystem.update(elapsed);
        renderingSystem.update(elapsed);
        
        start = end;
        end = clock.now();
    }
}
