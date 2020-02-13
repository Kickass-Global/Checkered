//
// Created by Jackson Cougar Wiebe on 2/13/2020.
//

#include <Events/Events.h>
#include <ComponentEvent.h>
#include <Dirty.h>
#include <Vehicle.h>
#include "vehiclesystem.hpp"

void Engine::vehicleSystem::initialize() {
    SystemInterface::initialize();
}
void Engine::vehicleSystem::update(Engine::deltaTime) {
    for (const auto &vehicle : Component::Index::entitiesOf(Component::ClassId::Vehicle)) {
        // check to see if we need to create a new phyx vehicle...
        if (vehicle.hasChildComponent(Component::Dirty::id())) {
            onVehicleCreated(vehicle);
        }
        if (vehicle.data<Component::Vehicle>()->model)
            vehicle.data<Component::Vehicle>()->model.attachExistingComponent(Engine::createComponent<Component::WorldTransform>()->id());
    }
}
