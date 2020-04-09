//
// Created by Jackson Cougar Wiebe on 2/13/2020.
//

#include "vehiclesystem.hpp"
#include "WorldTransform.h"
#include "glm/gtx/matrix_decompose.hpp"
#include <EngineDebug.hpp>
#include <EventDelegate.h>
#include <Events/Events.h>
#include <Vehicle.h>
#include <physicspacket.hpp>
#include <tags.h>

void Engine::vehicleSystem::initialize() { SystemInterface::initialize(); }

namespace physx {
std::ostream &operator<<(std::ostream &out, PxVec3 vec) {
  return out << vec.x << " " << vec.y << " " << vec.z;
}
} // namespace physx
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

      auto number_of_shapes =
          vehicle->pxVehicle->getRigidDynamicActor()->getNbShapes();
      std::vector<physx::PxShape *> shapes(number_of_shapes);
      vehicle->pxVehicle->getRigidDynamicActor()->getShapes(shapes.data(),
                                                            number_of_shapes);

      if (number_of_shapes >= 4) {
        vehicle->front_right_wheel->setTransform(
            vehicle->pxVehicle->getRigidDynamicActor()->getGlobalPose() *
            shapes[0]->getLocalPose());
        vehicle->front_left_wheel->setTransform(
            vehicle->pxVehicle->getRigidDynamicActor()->getGlobalPose() *
            shapes[1]->getLocalPose());
        vehicle->back_right_wheel->setTransform(
            vehicle->pxVehicle->getRigidDynamicActor()->getGlobalPose() *
            shapes[2]->getLocalPose());
        vehicle->back_left_wheel->setTransform(
            vehicle->pxVehicle->getRigidDynamicActor()->getGlobalPose() *
            shapes[3]->getLocalPose());
      }

      auto is_flipped = physx::PxVec3{0, 1, 0}.dot(
                            vehicle->pxVehicle->getRigidDynamicActor()
                                ->getGlobalPose()
                                .transform(physx::PxVec3{0, 1, 0})) < 0.15;
      if (is_flipped) {
        // if the vehicle is flipped apply some force to right it.
        auto T = vehicle->pxVehicle->getRigidDynamicActor()->getGlobalPose();
        auto local_up = T.rotate(physx::PxVec3{0, 1, 0});
        auto global_up = physx::PxVec3{0, 1, 0};
        auto force = 1000 * global_up - local_up;
        vehicle->pxVehicle->getRigidDynamicActor()->addTorque(force);

        Engine::log<module, medium>("Applying torque: ", force);
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
