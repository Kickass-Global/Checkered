//
// Created by Jackson Cougar Wiebe on 4/3/2020.
//

#ifndef ENGINE_WAYPOINT_HPP
#define ENGINE_WAYPOINT_HPP

struct WaypointArrow : public ComponentBase {
  std::shared_ptr<PaintedMesh> m_mesh;
  glm::vec3 waypoint_location = {0, 0, 0};
  std::shared_ptr<Vehicle> target_vehicle;

  WaypointArrow() {
    const std::shared_ptr<Program> &basic_program =
        getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Program>(
            "Assets/Programs/basic.json");
    auto material = getEngine()->createComponent<Material>(basic_program);
    auto mesh = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
        "Assets/Meshes/arrow.obj");
    m_mesh = getEngine()->createComponent<PaintedMesh>(mesh, material);
  }

  void tick(float time) {

    auto waypoints = getEngine()
                         ->getSubSystem<EngineStore>()
                         ->getRoot()
                         .getComponentsOfType<Waypoint>();

    if (!waypoints.empty()) {
      waypoint_location = waypoints[0]->actor->position;
      auto direction =
          -glm::normalize(waypoint_location - target_vehicle->position);
      auto up = glm::vec3{0, 1, 0};
      auto p = glm::cross(direction, up);
      glm::mat4 T = glm::mat4(
          glm::vec4(p, 0), glm::vec4(up, 0), glm::vec4(direction, 0),
          glm::vec4{target_vehicle->position.x, target_vehicle->position.y + 2,
                    target_vehicle->position.z, 1});

      glm::mat4 T_offset = glm::translate(glm::vec3(0, 0, 0));

      m_mesh->getStore().eraseComponentsOfType<WorldTransform>();
      m_mesh->getStore().emplaceComponent<WorldTransform>(T * T_offset);
    }
  }
};
#include "Rendering/text.hpp"
#include "scenariointerface.hpp"
#include <Billboard.h>
#include <Engine.h>
#include <Pipeline/Library.h>
#include <Vehicle.h>
#include <WorldTransform.h>
#include <optional>
#include <scenery.hpp>
#endif // ENGINE_WAYPOINT_HPP
