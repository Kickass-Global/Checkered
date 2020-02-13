//
// Created by Jackson Cougar Wiebe on 2/13/2020.
//

#include <Events/Events.h>
#include <ComponentEvent.h>
#include <Dirty.h>
#include <Vehicle.h>
#include "vehiclesystem.hpp"
#include <physicshandler.hpp>

void Engine::vehicleSystem::initialize() {
    SystemInterface::initialize();
}
void Engine::vehicleSystem::update(Engine::deltaTime) {
	for (const auto &vehicle : Component::Index::entitiesOf(Component::ClassId::Vehicle)) {

		// check to see if we need to create a new phyx vehicle...
		if (vehicle.hasChildComponent(Component::Dirty::id()) && !vehicle.data<Component::Vehicle>()->pxVehicle) {
			onVehicleCreated(vehicle);
		}

		auto is_dirty = vehicle.hasChildComponent(Component::Dirty::id());
		auto physicsUpdates = vehicle.childComponentsOfClass(Component::ClassId::PhysicsPacket);
		auto has_physics_update = !physicsUpdates.empty();

		auto meta = vehicle.data<Component::Vehicle>();

		if (has_physics_update) {

			auto physx_data = physicsUpdates.begin()->data<Component::PhysicsPacket>();
			auto T = glm::translate(physx_data->position) * glm::mat4_cast(physx_data->rotation);

			if (vehicle.data<Component::Vehicle>()->model) {
				vehicle.data<Component::Vehicle>()->model.attachExistingComponent(Engine::createComponent<Component::WorldTransform>(T)->id());
			}
		}

		vehicle.destroyComponentsOfType(Component::ClassId::PhysicsPacket);
	}
}
