// Engine.cpp : Defines the entry point for the application.
//

#include <functional>

#include "main.h"
#include "Systems/Pipeline/EntityLoader.h"
#include "Systems/Damage/damagesystem.hpp"
#include <PxPhysicsAPI.h>
#include <Vehicle/vehiclesystem.hpp>
#include <Vehicle.h>

#include "glm/gtx/transform.hpp"
#include "Systems/Component/scenecomponentsystem.hpp"
#include "Systems/systeminterface.hpp"
#include "Engine.h"

int main() {

    // region initialize engine systems
    using namespace Engine;

    auto running = true;

    auto physicsSystem = Engine::addSystem<Physics::PhysicsSystem>();
    auto vehicleSystem = Engine::addSystem<Engine::vehicleSystem>();

    vehicleSystem.onVehicleCreated += physicsSystem.onVehicleCreatedHandler;

    Engine::addSystem<Component::SceneComponentSystem>();
    Engine::addSystem<Engine::DamageSystem>();
    auto cameraSystem = Engine::addSystem<Camera::CameraSystem>();
    auto renderingSystem = Engine::addSystem<Rendering::RenderingSystem>();
    auto liveReloadSystem = Engine::addSystem<Debug::LiveReloadSystem>();
    auto inputSystem = Engine::addSystem<Input::InputSystem>();

    // hookup inputs from current window
    inputSystem.initialize(renderingSystem.getWindow());

    // hookup key press event with camera system

    Engine::addSystem<Engine::EventSystem>();

    Input::InputSystem::onKeyPress += cameraSystem.onKeyPressHandler;
    Input::InputSystem::onKeyDown += cameraSystem.onKeyDownHandler;
    Input::InputSystem::onKeyUp += cameraSystem.onKeyUpHandler;

    Input::InputSystem::onKeyPress += physicsSystem.onKeyPressHandler;
    Input::InputSystem::onKeyDown += physicsSystem.onKeyDownHandler;
    Input::InputSystem::onKeyUp += physicsSystem.onKeyUpHandler;

    Rendering::RenderingSystem::onWindowSizeChanged += cameraSystem.onWindowSizeHandler;
    //endregion

    // setup plane for ground

    auto ground_object = Engine::createComponent<Component::SceneComponent>();
    auto quad_mesh = Pipeline::Library::getAsset("Assets/Meshes/plane.obj", Component::ClassId::Mesh);
    quad_mesh.data<Component::Mesh>()->shader = Pipeline::Library::getAsset(
        "Assets/Programs/basic.json",
        Component::ClassId::Program
    );
    quad_mesh.attachExistingComponent(Component::Dirty::id());
    Engine::nameComponent(quad_mesh, "quad-boi");

    ground_object->attachComponent(quad_mesh);
    ground_object->id().attachExistingComponent(Component::Dirty::id());
    ground_object->id().attachExistingComponent(Component::Visible::id());

    // region setup damage model for player vehicle

    auto damage_object = Engine::createComponent<Component::SceneComponent>();
    damage_object->m_localTransform = glm::translate(
            glm::vec3(0.0f, 0.0f, 0.0f));

    auto damage_model = Engine::createComponent<Component::Model>();

    auto sphere_0_mesh = Pipeline::Library::getAsset("Assets/Meshes/Cartoon_Lowpoly_Car.obj", Component::ClassId::Mesh);

    sphere_0_mesh.data<Component::Mesh>()->shader = Pipeline::Library::getAsset(
            "Assets/Programs/basic.json",
            Component::ClassId::Program
    );

    damage_model->parts.push_back(Component::Model::Part{});
    damage_model->parts[0].variations.push_back(Component::Model::Variation{2000000, sphere_0_mesh});
    damage_model->id().attachExistingComponent(Component::Dirty::id());

    damage_object->id().attachExistingComponent(damage_model->id());
    damage_object->id().attachExistingComponent(Component::Dirty::id());
    damage_object->id().attachExistingComponent(Component::Visible::id());

    physicsSystem.link(damage_object->id(), physicsSystem.getVehicleActor());

    auto ai_vehicle = Engine::createComponent<Component::Vehicle>();
    auto ai_damage_model = Engine::createComponent<Component::Model>();

    ai_damage_model->parts.push_back(Component::Model::Part{});
    ai_damage_model->parts[0].variations.push_back(Component::Model::Variation{2000000, sphere_0_mesh});
    ai_damage_model->id().attachExistingComponent(Component::Dirty::id());

    ai_vehicle->model = ai_damage_model->id();

    // endregion

    // make a default camera
    auto camera = Engine::createComponent<Component::Camera>();
    camera->id().attachExistingComponent(Component::Dirty::id());
    camera->target = damage_object->id();

    // region initialize game-clocks
    using namespace std::chrono;
    typedef duration<float> fmilli; // define this to get float values;
    auto start = high_resolution_clock::now();
    auto end = start + milliseconds(1); // do this so physx doesn't complain
    // endregion

    while (running) {

        // region before update
        fmilli delta = end - start;
        deltaTime elapsed = static_cast<deltaTime>(duration_cast<milliseconds>(delta).count());
        // endregion

        // todo remove these hacks...
        quad_mesh.attachExistingComponent(Component::Visible::id());
        quad_mesh.attachExistingComponent(Engine::createComponent<Component::WorldTransform>()->id());

        ai_vehicle->id().attachExistingComponent(Component::Visible::id());
        for (const auto &system : Engine::systems()) {
            system->update(elapsed);
        }

        // region after update
        start = end;
        end = high_resolution_clock::now();
        //endregion
    }
}
