// Engine.cpp : Defines the entry point for the application.
//

#include <functional>

#include "Systems/Rendering/RenderingSystem.h"
#include "Systems/Pipeline/ShaderLoader.h"
#include "Systems/Rendering/RenderingBatch.h"
#include "Systems/Input/InputSystem.h"
#include "Systems/Camera/CameraSystem.h"
#include "Systems/Pipeline/MeshLoader.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Components/GameObject.h"
#include "Systems/Debug/LiveReloadSystem.h"
#include "Components/ComponentBase.h"

int main() {

    using namespace Engine;
    
    auto running = true;

    Rendering::RenderingSystem renderingSystem;
    renderingSystem.initialize();

    Debug::LiveReloadSystem liveReloadSystem;

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

    // simulate loading a complex game object

    Pipeline::MeshLoader meshLoader;
    auto box = Engine::createComponent(
            meshLoader.load("Assets/Meshes/box.obj")
    );

    auto box_object = Engine::createComponent<Component::GameObject>();
    box_object->attachComponent(box->id());

    liveReloadSystem.watch("Assets/Meshes/box.obj", Component::ClassId::Mesh, box->id());
    liveReloadSystem.onAssetModified += [&renderingSystem](
            const std::string& filename,
            const Component::ClassId& classId,
            const Component::ComponentId& componentId
            )
    {
        if(classId == Component::ClassId::Mesh)
        {
            Pipeline::MeshLoader meshLoader;
            auto meshComponent = Engine::replaceComponent(componentId,
                    meshLoader.load(filename)
            );

            renderingSystem.buffer(*meshComponent);
            // unload existing data
            // load new data
        }
    };

    // send data to rendering system for drawing...
    renderingSystem.buffer(*box);

    // make a default camera
    auto camera = Engine::createComponent<Component::Camera>();

    // setup a game clock
    std::chrono::high_resolution_clock clock;
   
    auto start = clock.now();
    auto end = start;

    //system_calls::loggingLevel = system_calls::Importance::low;

    while (running) {

        // frametime is a measure of how 'on-time' a frame is... <1 early, >1 late.
        frametime elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 16.6666667f;
        log<module, Importance::low>("Frametime: ", elapsed);

        liveReloadSystem.update(elapsed);
        cameraSystem.update(elapsed);
        renderingSystem.update(elapsed);
        
        start = end;
        end = clock.now();
    }
}
