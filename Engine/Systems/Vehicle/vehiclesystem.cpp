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
#include <EngineDebug.hpp>

void Engine::vehicleSystem::initialize() { SystemInterface::initialize(); }

namespace physx {
    std::ostream& operator<<(std::ostream& out, PxVec3 vec) {
        return out << vec.x << " " << vec.y << " " << vec.z;
    }
}
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

      auto is_flipped = physx::PxVec3{ 0,1,0 }.dot(vehicle->pxVehicle->getRigidDynamicActor()->getGlobalPose().transform(physx::PxVec3{ 0,1,0 })) < 0.15;
      if (is_flipped) {
          auto T = vehicle->pxVehicle->getRigidDynamicActor()->getGlobalPose();
          auto local_up = T.rotate(physx::PxVec3{ 0,1,0 });
          auto global_up = physx::PxVec3{ 0,1,0 };
          auto force =1000 *  global_up - local_up;
          vehicle->pxVehicle->getRigidDynamicActor()->addTorque(force);

          Engine::log<module, high>("Applying torque: ", force);
      }

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
