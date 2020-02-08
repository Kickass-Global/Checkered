// Engine.cpp : Defines the entry point for the application.
//

#include <functional>

#include "main.h"
#include "Systems/Pipeline/EntityLoader.h"
#include "Systems/Damage/damagesystem.hpp"
#include <PxPhysicsAPI.h>

#include "glm/gtx/transform.hpp"
#include "Systems/Component/scenecomponentsystem.hpp"
#include "Systems/systeminterface.hpp"

int main() {

    using namespace Engine;

    auto running = true;

    Engine::addSystem(std::make_shared<Physics::PhysicsSystem>());
    Engine::addSystem(std::make_shared<Component::SceneComponentSystem>());

    Engine::addSystem(std::make_shared<Engine::DamageSystem>());
    auto renderingSystem = std::make_shared<Rendering::RenderingSystem>();
    Engine::addSystem(renderingSystem);

    auto liveReloadSystem = std::make_shared<Debug::LiveReloadSystem>();
    Engine::addSystem(liveReloadSystem);

    auto inputSystem = std::make_shared<Input::InputSystem>();
    Engine::addSystem(inputSystem);

    // hookup inputs from current window
    inputSystem->initialize(renderingSystem->getWindow());

    // hookup key press event with camera system
    auto cameraSystem = std::make_shared<Camera::CameraSystem>();
    Engine::addSystem(cameraSystem);
    Engine::addSystem(std::make_shared<Engine::EventSystem>());

    Input::InputSystem::onKeyPress += cameraSystem->onKeyPressHandler;
    Input::InputSystem::onKeyDown += cameraSystem->onKeyDownHandler;
    Input::InputSystem::onKeyUp += cameraSystem->onKeyUpHandler;

    Rendering::RenderingSystem::onWindowSizeChanged += cameraSystem->onWindowSizeHandler;

    auto damage_object = Engine::createComponent<Component::SceneComponent>("player");
    damage_object->m_localTransform = glm::translate(
            glm::vec3(0.0f, 0.0f, 10.0f));

    auto damage_model = Engine::createComponent<Component::Model>("model");

    auto sphere_0_mesh = Pipeline::Library::getAsset("Assets/Meshes/sphere_0.obj", Component::ClassId::Mesh);
    auto sphere_1_mesh = Pipeline::Library::getAsset("Assets/Meshes/sphere_1.obj", Component::ClassId::Mesh);
    auto sphere_2_mesh = Pipeline::Library::getAsset("Assets/Meshes/sphere_2.obj", Component::ClassId::Mesh);
    auto sphere_3_mesh = Pipeline::Library::getAsset("Assets/Meshes/sphere_3.obj", Component::ClassId::Mesh);

    sphere_0_mesh.data<Component::Mesh>()->shader = Pipeline::Library::getAsset(
            "Assets/Programs/basic.json",
            Component::ClassId::Program
    );
    sphere_1_mesh.data<Component::Mesh>()->shader = Pipeline::Library::getAsset(
            "Assets/Programs/basic.json",
            Component::ClassId::Program
    );
    sphere_2_mesh.data<Component::Mesh>()->shader = Pipeline::Library::getAsset(
            "Assets/Programs/basic.json",
            Component::ClassId::Program
    );
    sphere_3_mesh.data<Component::Mesh>()->shader = Pipeline::Library::getAsset(
            "Assets/Programs/basic.json",
            Component::ClassId::Program
    );

    damage_model->parts.push_back(Component::Model::Part{});
    damage_model->parts[0].variations.push_back(Component::Model::Variation{2, sphere_0_mesh});
    damage_model->parts[0].variations.push_back(Component::Model::Variation{5, sphere_1_mesh});
    damage_model->parts[0].variations.push_back(Component::Model::Variation{8, sphere_2_mesh});
    damage_model->parts[0].variations.push_back(Component::Model::Variation{300000, sphere_3_mesh});
    damage_model->id().attachExistingComponent(Component::Dirty::id());

    damage_object->id().attachExistingComponent(sphere_0_mesh);
    damage_object->id().attachExistingComponent(damage_model->id());
    damage_object->id().attachExistingComponent(Component::Dirty::id());
    damage_object->id().attachExistingComponent(Component::Visible::id());

    std::function<void(const Component::EventArgs<int> &)> onKeyPress
            = [damage_model](auto &args) {

                auto key = std::get<0>(args.values);
                Engine::log(key, " was pressed");
                if (key == GLFW_KEY_D) {
                    auto damage = Engine::createComponent<Component::Damage>();
                    damage->damage_amount = 1;
                    damage_model->id().attachExistingComponent(damage->id());
                }
                if (key == GLFW_KEY_F) {
                    auto damage = Engine::createComponent<Component::Damage>();
                    damage->damage_amount = -1;
                    damage_model->id().attachExistingComponent(damage->id());
                }
            };

    auto debugHandler = Engine::EventSystem::createHandler(onKeyPress);
    Input::InputSystem::onKeyPress += debugHandler;


    // make a default camera
    auto camera = Engine::createComponent<Component::Camera>();
    camera->id().attachExistingComponent(Component::Dirty::id());

    // setup a game clock

    auto start = std::chrono::high_resolution_clock::now();
    auto end = start;

    while (running) {

        deltaTime elapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                        end - start
                ).count();

        for (const auto &system : Engine::systems()) {
            system->update(elapsed);
        }

        start = end;
        end = std::chrono::high_resolution_clock::now();

    }
}
