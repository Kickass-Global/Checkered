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
#include "glm/gtx/matrix_decompose.hpp"

void Engine::vehicleSystem::initialize() {
    SystemInterface::initialize();
}
void Engine::vehicleSystem::update(Engine::deltaTime) {

    auto vehicles = Component::Index::entitiesOf<Component::Vehicle>();
    for (const auto &vehicle : vehicles) {

        // check to see if we need to create a new phyx vehicle...
        auto is_dirty = vehicle.hasTag<Component::Dirty>(true);
        if (is_dirty && !vehicle.data<Component::Vehicle>()->pxVehicle) {
            onVehicleCreated(vehicle);
        }

        auto physicsUpdates = vehicle.childComponentsOfClass(Component::ClassId::PhysicsPacket);
        auto has_physics_update = !physicsUpdates.empty();

        auto meta = vehicle.data<Component::Vehicle>();

        if (has_physics_update) {

            auto physx_data = physicsUpdates.begin()->data<Component::PhysicsPacket>();

            meta->rotation = physx_data->rotation;
            meta->position = physx_data->position;

            if (vehicle.data<Component::Vehicle>()->model) {
                vehicle.data<Component::Vehicle>()->model.attachExistingComponent(
                        Engine::createComponent<Component::WorldTransform>(meta->world_transform())->id());
            }
        }

        vehicle.destroyComponentsOfType(Component::ClassId::PhysicsPacket);
    }
}
