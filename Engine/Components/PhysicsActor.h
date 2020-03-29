//
// Created by Jackson Cougar Wiebe on 3/1/2020.
//

#ifndef ENGINE_PHYSICSACTOR_H
#define ENGINE_PHYSICSACTOR_H

#include "Mesh.h"
#include "TransformData.hpp"
#include <Component.h>
#include <Engine.h>
#include <PxPhysicsAPI.h>
#include <SceneComponent.h>
#include <memory>

namespace Component {
class PhysicsActor : public ComponentBase {

public:
  EventDelegate<PhysicsActor *, PhysicsActor *> onBeginOverlap{
      "onBeginOverlap"};
  EventDelegate<PhysicsActor *, PhysicsActor *> onEndOverlap{"onEndOverlap"};
  EventDelegate<PhysicsActor *, PhysicsActor *> onCollision{"onCollision"};
  EventDelegate<class PhysicsActor *, class PhysicsActor *> onHit{"onHit"};


  std::shared_ptr<SceneComponent> node =
      getEngine()->createComponent<SceneComponent>(nullptr);

  // this will be used to generate the collision bounds, may be different than
  // the visual mesh.
  std::shared_ptr<Mesh> mesh;
  physx::PxRigidActor *actor;
  TransformData worldTransform{};
  glm::vec3 position = {0, 0, 0};
  glm::quat rotation = glm::quat(1, 0, 0, 0);

  enum Type {
    Ground,
    DynamicObject,
    StaticObject,
    TriggerVolume
  } type = StaticObject;

  glm::mat4 getWorldTransform();

  void update() {
    // update children poses to match parent
    node->m_localTransform = getWorldTransform();
  }

  explicit PhysicsActor(std::shared_ptr<Component::Mesh> &mesh);
};
} // namespace Component

#endif // ENGINE_PHYSICSACTOR_H
