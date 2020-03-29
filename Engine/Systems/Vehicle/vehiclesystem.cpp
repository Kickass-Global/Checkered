//
// Created by Jackson Cougar Wiebe on 2/13/2020.
//

#include "vehiclesystem.hpp"
#include "WorldTransform.h"
#include "glm/gtx/matrix_decompose.hpp"
#include <EventDelegate.h>
#include <Events/Events.h>
#include <Vehicle.h>
#include <physicspacket.hpp>
#include <tags.h>

void Engine::vehicleSystem::initialize() { SystemInterface::initialize(); }
void Engine::vehicleSystem::update(Engine::deltaTime) {

  auto vehicles = getEngine()
                      ->getSubSystem<EngineStore>()
                      ->getRoot()
                      .getComponentsOfType<Component::Vehicle>();
  for (auto &vehicle : vehicles) {

    if (!vehicle->pxVehicle) {
      onVehicleCreated(vehicle);
    }

    auto physicsUpdates =
        vehicle->getChildren().getComponentsOfType<Component::WorldTransform>();

    if (!physicsUpdates.empty()) {

      auto physx_data = physicsUpdates[0];
      vehicle->rotation = physx_data->rotation;
      vehicle->position = physx_data->position;

      if (vehicle->model) {
        vehicle->model->transform = vehicle->world_transform();
        vehicle->model->is_outdated = true;
        vehicle->getStore().eraseComponentsOfType<WorldTransform>();
        vehicle->getStore().emplaceComponent<WorldTransform>(
            vehicle->physx_transform()); // FOR CAMERA
      }
    }
  }
}
