//
// Created by Jackson Cougar Wiebe on 2/13/2020.
//

#include <Events/Events.h>
#include <EventDelegate.h>
#include <tags.h>
#include <Vehicle.h>
#include "vehiclesystem.hpp"
#include "WorldTransform.h"
#include <physicspacket.hpp>

void Engine::vehicleSystem::initialize() {
    SystemInterface::initialize();
}
void Engine::vehicleSystem::update(Engine::deltaTime) {
    for (const auto &vehicle : Component::Index::entitiesOf<Component::Vehicle>()) {

        // check to see if we need to create a new phyx vehicle...
        auto is_dirty = vehicle.hasTag<Component::Dirty>();
        if (is_dirty && !vehicle.data<Component::Vehicle>()->pxVehicle) {
            onVehicleCreated(vehicle);
        }

        auto physicsUpdates = vehicle.childComponentsOfClass(Component::ClassId::PhysicsPacket);
        auto has_physics_update = !physicsUpdates.empty();

        auto meta = vehicle.data<Component::Vehicle>();

        if (has_physics_update) {

            auto physx_data = physicsUpdates.begin()->data<Component::PhysicsPacket>();
            auto T = glm::translate(physx_data->position) * glm::mat4_cast(physx_data->rotation);
            meta->world_transform = T;

            if (vehicle.data<Component::Vehicle>()->model) {
                vehicle.data<Component::Vehicle>()->model.attachTemporaryComponent(
                        Engine::createComponent<Component::WorldTransform>(T)->id(), 1);
            }
        }
    }
}
