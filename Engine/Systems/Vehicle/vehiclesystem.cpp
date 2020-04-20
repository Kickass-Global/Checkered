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
}// namespace physx
void Engine::vehicleSystem::update(Engine::deltaTime elapsed) {

  auto vehicles =
      getEngine()->getSubSystem<EngineStore>()->getRoot().getComponentsOfType<Component::Vehicle>();
  for (auto &vehicle : vehicles) {

    if (!vehicle->pxVehicle) { onVehicleCreated(vehicle); }

    auto physicsUpdates = vehicle->getChildren().getComponentsOfType<Component::WorldTransform>();

    if (!physicsUpdates.empty()) {

      auto physx_data = physicsUpdates[0];
      vehicle->rotation = physx_data->rotation;
      vehicle->position = physx_data->position;

      auto number_of_shapes = vehicle->pxVehicle->getRigidDynamicActor()->getNbShapes();
      std::vector<physx::PxShape *> shapes(number_of_shapes);
      vehicle->pxVehicle->getRigidDynamicActor()->getShapes(shapes.data(), number_of_shapes);

      auto T = vehicle->pxVehicle->getRigidDynamicActor()->getGlobalPose();


      for (int i = 0; i < std::min<size_t>(vehicle->model->parts.size(), number_of_shapes); i++) {
        vehicle->model->parts[i].transform = convert_from(shapes[i]->getLocalPose());
      }

      //if (number_of_shapes >= 4 && vehicle->front_right_wheel) {
      //  vehicle->front_right_wheel->transform = convert_from(shapes[0]->getLocalPose());
      //  vehicle->front_left_wheel->transform = convert_from(shapes[1]->getLocalPose());
      //  vehicle->back_right_wheel->transform = convert_from(shapes[2]->getLocalPose());
      //  vehicle->back_left_wheel->transform = convert_from(shapes[3]->getLocalPose());
      //}

      auto is_flipped = physx::PxVec3{0, 1, 0}.dot(
                            vehicle->pxVehicle->getRigidDynamicActor()->getGlobalPose().transform(
                                physx::PxVec3{0, 1, 0})) < 0.15;

      static auto time_flipped = elapsed;
      if (is_flipped) {
        time_flipped += elapsed;

        // if the vehicle is flipped apply some force to right it.
        auto T = vehicle->pxVehicle->getRigidDynamicActor()->getGlobalPose();
        auto local_up = T.rotate(physx::PxVec3{0, 1, 0});
        auto global_up = physx::PxVec3{0, 1, 0};
        auto force =
            time_flipped * 100 * (global_up - local_up + physx::PxVec3(1, 0, 0)).getNormalized();

        vehicle->pxVehicle->getRigidDynamicActor()
                         ->addTorque(force);

        Engine::log<module, high>("Applying torque: ", force);
      } else {
        time_flipped = 0;
      }

      if (vehicle->model) {
        vehicle->model->transform = vehicle->physx_transform();
        vehicle->model->is_outdated = true;
        vehicle->getStore().eraseComponentsOfType<WorldTransform>();
        vehicle->getStore().emplaceComponent<WorldTransform>(
            vehicle->physx_transform());// FOR CAMERA
      }
    }
  }
}
