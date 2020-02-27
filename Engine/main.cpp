// Engine.cpp : Defines the entry point for the application.
//

#include <functional>

#include "main.h"
#include "Systems/Pipeline/EntityLoader.h"
#include "Systems/Damage/damagesystem.hpp"
#include <PxPhysicsAPI.h>
#include <Vehicle/vehiclesystem.hpp>
#include <Vehicle.h>
#include "Passenger.h"

#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include "Systems/Component/scenecomponentsystem.hpp"
#include "Systems/systeminterface.hpp"
#include "Engine.h"
#include "WorldTransform.h"
#include <Events/Events.h>
#include <texture.hpp>
#include <fstream>
#include <material.hpp>

int main() {

    // region initialize engine systems
    using namespace Engine;

    auto running = true;

    auto physicsSystem = Engine::addSystem<Physics::PhysicsSystem>();
    auto vehicleSystem = Engine::addSystem<Engine::vehicleSystem>();


    vehicleSystem->onVehicleCreated += physicsSystem->onVehicleCreatedHandler;
    auto index = Engine::addSystem<Component::Index>();
    index->order = 2;

    Engine::addSystem<Component::SceneComponentSystem>();
    Engine::addSystem<Engine::DamageSystem>();
    auto cameraSystem = Engine::addSystem<::Camera::CameraSystem>();
    auto renderingSystem = Engine::addSystem<Rendering::RenderingSystem>();
    auto liveReloadSystem = Engine::addSystem<Debug::LiveReloadSystem>();
    auto inputSystem = Engine::addSystem<Input::InputSystem>();

    auto hornSystem = Engine::addSystem<Horn::hornSystem>();
    // hookup inputs from current window
    inputSystem->initialize(renderingSystem->getWindow());

    // hookup key press event with camera system
    auto eventSystem = Engine::addSystem<Engine::EventSystem>();
    eventSystem->order = 0;
    renderingSystem->order = 2;

    Input::InputSystem::onKeyPress += cameraSystem->onKeyPressHandler;
    Input::InputSystem::onKeyDown += cameraSystem->onKeyDownHandler;
    Input::InputSystem::onKeyUp += cameraSystem->onKeyUpHandler;

    Input::InputSystem::onKeyPress += physicsSystem->onKeyPressHandler;
    Input::InputSystem::onKeyDown += physicsSystem->onKeyDownHandler;
    Input::InputSystem::onKeyUp += physicsSystem->onKeyUpHandler;

    Input::InputSystem::onKeyPress += hornSystem->onKeyPressHandler;
    Input::InputSystem::onKeyDown += hornSystem->onKeyDownHandler;
    Input::InputSystem::onKeyUp += hornSystem->onKeyUpHandler;

    Rendering::RenderingSystem::onWindowSizeChanged += cameraSystem->onWindowSizeHandler;
    //endregion

    // setup the ground mesh

    auto ground_object = Engine::createComponent<Component::SceneComponent>();
    auto quad_mesh = Pipeline::Library::getAsset("Assets/Meshes/plane.obj", Component::ClassId::Mesh);
    quad_mesh.data<Component::Mesh>()->shader = Pipeline::Library::getAsset(
            "Assets/Programs/checker.json",
            Component::ClassId::Program
    );

    quad_mesh.addTag<Component::Dirty>();
    quad_mesh.addTag<Component::Visible>();
    Engine::nameComponent(quad_mesh, "quad-boi");

    ground_object->id().attachExistingComponent(quad_mesh);
    ground_object->id().addTag<Component::Dirty>();
    ground_object->id().addTag<Component::Visible>();

    // setup the mesh used for the cars...


    auto car_mesh = Pipeline::Library::getAsset("Assets/Meshes/cop_mesh.fbx", Component::ClassId::Mesh);
    car_mesh.addTag<Component::Dirty>();
    car_mesh.addTag<Component::Visible>();
    car_mesh.attachTemporaryComponent(Engine::createComponent<Component::WorldTransform>()->id(), 1);
    car_mesh.data<Component::Mesh>()->shader = Pipeline::Library::getAsset(
            "Assets/Programs/basic.json",
            Component::ClassId::Program
    );
    Engine::nameComponent(car_mesh, "car-gal");

    auto material = Engine::createComponent<Component::Material>();
    auto diffuse = Engine::createComponent<Component::Texture>("Assets/Textures/Vehicle_Taxi.png");
    material->textures.push_back(diffuse->id());

    car_mesh.data<Component::Mesh>()->material = material->id();

    // setup the vehicle for the player...

    auto player_vehicle = Engine::createComponent<Component::Vehicle>();
    auto player_damage_model = Engine::createComponent<Component::Model>();

    player_damage_model->parts.push_back(Component::Model::Part{});
    player_damage_model->parts[0].variations.push_back(Component::Model::Variation{2000000, car_mesh});

    player_vehicle->model = player_damage_model->id();
    player_vehicle->world_transform = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f))
                                      * glm::rotate(180.0f, glm::vec3(0, 1, 0))
                                      * glm::translate(glm::vec3(0.0f, 0.0f, -40.0f));

    physicsSystem->playerVehicle = player_vehicle;

    // ai factory method...
    auto make_ai = [car_mesh](glm::mat4 world_transform = glm::mat4{1}) {

        auto ai_vehicle = Engine::createComponent<Component::Vehicle>();
        auto ai_damage_model = Engine::createComponent<Component::Model>();

        ai_damage_model->parts.push_back(Component::Model::Part{});
        ai_damage_model->parts[0].variations.push_back(Component::Model::Variation{2000000, car_mesh});

        glm::quat orientation;
        glm::vec3 scale, translation, skew;
        glm::vec4 perspective;
        glm::decompose(meta->world_transform, scale, orientation, translation, skew, perspective);

        ai_vehicle->model = ai_damage_model->id();
        ai_vehicle->world_transform = world_transform;

        return ai_vehicle;

    };


    //setup passenger
    auto passenger_entity = Engine::createComponent<Component::Passenger>();
    passenger_entity->initPassenger();

    // setup ai "brain"

    std::function<void(const Component::EventArgs<Component::ComponentId> &)> ai_tick_callback = [player_vehicle](
            const Component::EventArgs<Component::ComponentId> &args) {
        auto meta = std::get<0>(args.values).data<Component::Vehicle>();

        auto player_location = glm::normalize(player_vehicle->position); // translation vector of mat4
        auto ai_direction = glm::normalize(meta->world_transform[2]); // 'z' column vector of mat4 (i.e. forward)
        auto ai_location = glm::normalize(meta->position); // translation vector of mat4

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
    auto dim = 1l;
    for (int x = -dim; x <= dim; x++) {
        for (int y = -dim; y <= dim; y++) {

            auto ai_vehicle = make_ai(glm::translate(glm::vec3(x * 5, 0, y * 5)));
            ai_vehicle->tickHandler += ticker; // give them brain

        }
    }

    // make a default camera
    auto camera = Engine::createComponent<Component::Camera>();
    camera->target = player_vehicle->id(); // make camera follow player.

    // region initialize game-clocks
    using namespace std::chrono;
    typedef duration<float> floatMilliseconds; // define this to get float values;
    auto start = high_resolution_clock::now();
    auto end = start + milliseconds(1); // do this so physx doesn't complain about time being 0.
    // endregion

    Engine::sortSystems();

    while (running) {

        // region before update
        floatMilliseconds delta = end - start;
        deltaTime elapsed = static_cast<deltaTime>(duration_cast<milliseconds>(delta).count());
        // endregion

        Engine::EventSystem::onTick(elapsed);

        // todo remove these hacks...
        // force the ground to render...

        quad_mesh.addTag<Component::Visible>();
        quad_mesh.attachTemporaryComponent(Engine::createComponent<Component::WorldTransform>()->id(), 1);

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
