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

	const auto vehicles = Engine::getStore().getRoot().getComponentsOfType<Component::Vehicle>();
	for (const auto &vehicle : vehicles) {

		if (!vehicle->pxVehicle) {
			onVehicleCreated(vehicle.get());
		}

		auto physicsUpdates = vehicle->getChildren().getComponentsOfType<Component::PhysicsPacket>();
		vehicle->getChildren().eraseComponentsOfType<Component::PhysicsPacket>();

		if (!physicsUpdates.empty()) {

			auto physx_data = physicsUpdates[0];
			vehicle->rotation = physx_data->rotation;
			vehicle->position = physx_data->position;

			if (vehicle->model) {
				vehicle->model->transform = WorldTransform(vehicle->world_transform());
				vehicle->model->is_outdated = true;
				vehicle->getChildren().eraseComponentsOfType<WorldTransform>();
				vehicle->emplaceChildComponent<WorldTransform>(vehicle->world_transform()); // todo, eh
			}
		}
	}
}
