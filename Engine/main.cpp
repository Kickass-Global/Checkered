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
#include "glm/gtc/matrix_transform.hpp"

#include "Systems/Component/scenecomponentsystem.hpp"
#include "Systems/systeminterface.hpp"
#include "Engine.h"
#include <Events/Events.h>

int main() {

    // region initialize engine systems
    using namespace Engine;

    auto running = true;

    auto physicsSystem = Engine::addSystem<Physics::PhysicsSystem>();
    auto vehicleSystem = Engine::addSystem<Engine::vehicleSystem>();

    vehicleSystem->onVehicleCreated += physicsSystem->onVehicleCreatedHandler;

    Engine::addSystem<Component::SceneComponentSystem>();
    Engine::addSystem<Engine::DamageSystem>();
    auto cameraSystem = Engine::addSystem<Camera::CameraSystem>();
    auto renderingSystem = Engine::addSystem<Rendering::RenderingSystem>();
    auto liveReloadSystem = Engine::addSystem<Debug::LiveReloadSystem>();
    auto inputSystem = Engine::addSystem<Input::InputSystem>();

    // hookup inputs from current window
    inputSystem->initialize(renderingSystem->getWindow());

    // hookup key press event with camera system
    Engine::addSystem<Engine::EventSystem>();

    Input::InputSystem::onKeyPress += cameraSystem->onKeyPressHandler;
    Input::InputSystem::onKeyDown += cameraSystem->onKeyDownHandler;
    Input::InputSystem::onKeyUp += cameraSystem->onKeyUpHandler;

    Input::InputSystem::onKeyPress += physicsSystem->onKeyPressHandler;
    Input::InputSystem::onKeyDown += physicsSystem->onKeyDownHandler;
    Input::InputSystem::onKeyUp += physicsSystem->onKeyUpHandler;

    Rendering::RenderingSystem::onWindowSizeChanged += cameraSystem->onWindowSizeHandler;
    //endregion

    // setup the ground mesh

    auto ground_object = Engine::createComponent<Component::SceneComponent>();
    auto quad_mesh = Pipeline::Library::getAsset("Assets/Meshes/plane.obj", Component::ClassId::Mesh);
    quad_mesh.data<Component::Mesh>()->shader = Pipeline::Library::getAsset(
            "Assets/Programs/checker.json",
            Component::ClassId::Program
    );
    quad_mesh.attachExistingComponent(Component::Dirty::id());
    Engine::nameComponent(quad_mesh, "quad-boi");

    ground_object->attachComponent(quad_mesh);
    ground_object->id().attachExistingComponent(Component::Dirty::id());
    ground_object->id().attachExistingComponent(Component::Visible::id());

    // setup the mesh used for the cars...

    auto car_mesh = Pipeline::Library::getAsset("Assets/Meshes/Cartoon_Lowpoly_Car.obj", Component::ClassId::Mesh);
    car_mesh.attachExistingComponent(Component::Dirty::id());
    car_mesh.attachExistingComponent(Component::Visible::id());
    car_mesh.attachExistingComponent(Engine::createComponent<Component::WorldTransform>()->id());
    car_mesh.data<Component::Mesh>()->shader = Pipeline::Library::getAsset(
            "Assets/Programs/basic.json",
            Component::ClassId::Program
    );
    Engine::nameComponent(car_mesh, "car-gal");

    // setup the vehicle for the player...

    auto player_vehicle = Engine::createComponent<Component::Vehicle>();
    auto player_damage_model = Engine::createComponent<Component::Model>();

    player_damage_model->parts.push_back(Component::Model::Part{});
    player_damage_model->parts[0].variations.push_back(Component::Model::Variation{2000000, car_mesh});

    player_vehicle->model = player_damage_model->id();
    player_vehicle->world_transform = glm::translate(glm::vec3(0.0f, 0.0f, -40.0f));
    physicsSystem->playerVehicle = player_vehicle;

    // ai factory method...
    auto make_ai = [car_mesh](glm::mat4 world_transform = glm::mat4{1}) {

        auto ai_vehicle = Engine::createComponent<Component::Vehicle>();
        auto ai_damage_model = Engine::createComponent<Component::Model>();

        ai_damage_model->parts.push_back(Component::Model::Part{});
        ai_damage_model->parts[0].variations.push_back(Component::Model::Variation{2000000, car_mesh});

        ai_vehicle->model = ai_damage_model->id();
        ai_vehicle->world_transform = world_transform;

        return ai_vehicle;

    };

    // setup ai "brain"

    std::function<void(const Component::EventArgs<Component::ComponentId> &)> ai_tick_callback = [player_vehicle](
            const Component::EventArgs<Component::ComponentId> &args) {
        auto meta = std::get<0>(args.values).data<Component::Vehicle>();

        auto player_location = glm::normalize(player_vehicle->world_transform[3]); // translation vector of mat4
        auto ai_direction = glm::normalize(meta->world_transform[2]); // 'z' column vector of mat4 (i.e. forward)
        auto ai_location = glm::normalize(meta->world_transform[3]); // translation vector of mat4

        // check if the player is to the left or right of the ai.
        auto perpdot = [](auto v1, auto v2) {
            return v1.z * v2.x - v1.x * v2.z;
        };

        // check if the player is in front or behind the ai.
        auto heading = [p = glm::normalize(glm::vec3(player_location - ai_location)), b = glm::vec3(ai_direction)]() {
            return glm::dot(p, b);
        };

        meta->pxVehicleInputData.setDigitalSteerLeft(false);
        meta->pxVehicleInputData.setDigitalSteerRight(false);

        auto player_direction = perpdot(player_location - ai_location, ai_direction);

        // todo control throttle better...
        meta->pxVehicleInputData.setDigitalAccel(true);

        const auto ai_is_driving_away = heading() < 0.0f;
        const auto player_is_left = player_direction > 0;

        if (ai_is_driving_away) { // turn the ai around...

            meta->pxVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
            meta->pxVehicle->mDriveDynData.setUseAutoGears(false);

            if (player_is_left) meta->pxVehicleInputData.setDigitalSteerLeft(true);
            else meta->pxVehicleInputData.setDigitalSteerRight(true);

        } else { // driving towards player

            meta->pxVehicle->mDriveDynData.setUseAutoGears(true);

            const auto pointed_at_player = -0.3f <= player_direction && player_direction <= 0.3f;

            if (pointed_at_player) { /* drive straight */ }
            else { // steer towards player
                if (player_is_left) meta->pxVehicleInputData.setDigitalSteerLeft(true);
                else meta->pxVehicleInputData.setDigitalSteerRight(true);
            }

        }
    };
    Component::ComponentId ticker = Engine::EventSystem::createHandler(ai_tick_callback);

    // spawn some ai bois into the world

    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {

            auto ai_vehicle = make_ai(glm::translate(glm::vec3(x * 45, 0, y * 45)));
            ai_vehicle->tickHandler += ticker; // give them brain

        }
    }

    // make a default camera
    auto camera = Engine::createComponent<Component::Camera>();
    camera->id().attachExistingComponent(Component::Dirty::id());
    camera->target = player_vehicle->id(); // make camera follow player.

    // region initialize game-clocks
    using namespace std::chrono;
    typedef duration<float> floatMilliseconds; // define this to get float values;
    auto start = high_resolution_clock::now();
    auto end = start + milliseconds(1); // do this so physx doesn't complain about time being 0.
    // endregion

    while (running) {

        // region before update
        floatMilliseconds delta = end - start;
        deltaTime elapsed = static_cast<deltaTime>(duration_cast<milliseconds>(delta).count());
        // endregion

        // todo remove these hacks...
        // force the ground to render...
        quad_mesh.attachExistingComponent(Component::Visible::id());
        quad_mesh.attachExistingComponent(Engine::createComponent<Component::WorldTransform>()->id());

        //ai_vehicle->id().attachExistingComponent(Component::Visible::id());
        for (const auto &system : Engine::systems()) {
            system->update(elapsed);
        }

        // region after update
        start = end;
        end = high_resolution_clock::now();
        //endregion
    }
}
