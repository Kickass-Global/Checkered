#pragma once
#ifndef ENGINE_COMPONENTS_SCENERY_HPP
#define ENGINE_COMPONENTS_SCENERY_HPP

#include <Engine.h>
#include <Events/Events.h>
#include <Mesh.h>
#include <PhysicsActor.h>
#include <SceneComponent.h>
#include <glm/gtx/matrix_decompose.hpp>
#include <material.hpp>


namespace Component {

template <typename T> class Instance : public ComponentBase {
public:
  std::vector<std::shared_ptr<T>> instance_node;

  template <typename... Args> void add_instance_at(Args... args) {

    instance_node.push_back(getEngine()->createComponent<T>(args...));
  }
};

class DrivableScenery : public ComponentBase {
public:
  std::shared_ptr<PhysicsActor> actor;
  std::shared_ptr<PaintedMesh> mesh;

  DrivableScenery(glm::mat4 transform, std::shared_ptr<Mesh> mesh,
                  std::shared_ptr<Material> material,
                  std::shared_ptr<Mesh> collision_mesh)
      : mesh(getEngine()->createComponent<PaintedMesh>(mesh, material)),
        actor(getEngine()->createComponent<PhysicsActor>(collision_mesh)) {
    actor->type = PhysicsActor::Type::Ground;

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 position;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(transform, scale, rotation, position, skew, perspective);

    actor->position = position;
    actor->rotation = rotation;

    actor->node->getStore().addComponent(
        getEngine()->createComponent<SceneComponent>(actor->node, this->mesh));
  }

  DrivableScenery(glm::vec3 position, std::shared_ptr<Mesh> mesh,
                  std::shared_ptr<Material> material)
      : mesh(getEngine()->createComponent<PaintedMesh>(mesh, material)),
        actor(getEngine()->createComponent<PhysicsActor>(mesh)) {

    actor->type = PhysicsActor::Type::Ground;
    actor->position = position;
    actor->node->getStore().addComponent(
        getEngine()->createComponent<SceneComponent>(actor->node, this->mesh));
  }

  DrivableScenery(glm::vec3 position, std::shared_ptr<Mesh> mesh,
                  std::shared_ptr<Material> material,
                  std::shared_ptr<Mesh> collision_mesh)
      : mesh(getEngine()->createComponent<PaintedMesh>(mesh, material)),
        actor(getEngine()->createComponent<PhysicsActor>(collision_mesh)) {

    actor->type = PhysicsActor::Type::Ground;
    actor->position = position;
    actor->node->getStore().addComponent(
        getEngine()->createComponent<SceneComponent>(actor->node, this->mesh));
  }
};

class Obstacle : public ComponentBase {
public:
  std::shared_ptr<PhysicsActor> actor;
  std::shared_ptr<PaintedMesh> mesh;

  Obstacle(glm::vec3 position, std::shared_ptr<Mesh> mesh,
           std::shared_ptr<Material> material)
      : mesh(getEngine()->createComponent<PaintedMesh>(mesh, material)),
        actor(getEngine()->createComponent<PhysicsActor>(mesh)) {

    actor->type = PhysicsActor::Type::DynamicObject;
    actor->position = position;
    actor->node->getStore().addComponent(
        getEngine()->createComponent<SceneComponent>(actor->node, this->mesh));
  }

  Obstacle(glm::mat4 global_pose, std::shared_ptr<Mesh> mesh,
      std::shared_ptr<Material> material)
      : mesh(getEngine()->createComponent<PaintedMesh>(mesh, material)),
      actor(getEngine()->createComponent<PhysicsActor>(mesh)) {

      actor->type = PhysicsActor::Type::DynamicObject;

      glm::vec3 translation, scale, skew;
      glm::vec4 perspective;
      glm::quat orientation;
      glm::decompose(global_pose, scale, orientation, translation, skew, perspective);

      actor->position = translation;
      actor->rotation = orientation;

      actor->node->getStore().addComponent(
          getEngine()->createComponent<SceneComponent>(actor->node, this->mesh));
  }
};

class Waypoint : public ComponentBase {
public:
  std::shared_ptr<PhysicsActor> actor;
  std::shared_ptr<PaintedMesh> mesh;

  EventDelegate<PhysicsActor *> onEntered{"onEntered"};
  EventDelegate<PhysicsActor *> onExited{"onExited"};

  std::shared_ptr<EventHandler<PhysicsActor *, PhysicsActor *>>
      onActorOverlapBeginHandler;
  std::shared_ptr<EventHandler<PhysicsActor *, PhysicsActor *>>
      onActorOverlapEndHandler;

  Waypoint(glm::vec3 position, std::shared_ptr<Mesh> mesh,
           std::shared_ptr<Material> material);

  void
  onActorOverlapBegin(const EventArgs<PhysicsActor *, PhysicsActor *> &args);

  void
  onActorOverlapEnd(const EventArgs<PhysicsActor *, PhysicsActor *> &args);
};

/**
 * Scenery components are things that can be placed into the world and have
 * collision properties
 */
class Scenery : public ComponentBase {
public:
  std::shared_ptr<PhysicsActor> actor;
  std::shared_ptr<PaintedMesh> mesh;

  Scenery(glm::mat4 T, std::shared_ptr<Mesh> mesh,
      std::shared_ptr<Material> material)
      : mesh(getEngine()->createComponent<PaintedMesh>(mesh, material)),
      actor(getEngine()->createComponent<PhysicsActor>(mesh)) {

      glm::vec4 perspective;
      glm::vec3 skew, scale, position;
      glm::quat rotation;
      glm::decompose(T, scale, rotation, position, skew, perspective);

      actor->position = position;
      actor->rotation = rotation;
      actor->node->getStore().addComponent(
          getEngine()->createComponent<SceneComponent>(actor->node, this->mesh));
  }

  Scenery(glm::vec3 position, std::shared_ptr<Mesh> mesh,
          std::shared_ptr<Material> material)
      : mesh(getEngine()->createComponent<PaintedMesh>(mesh, material)),
        actor(getEngine()->createComponent<PhysicsActor>(mesh)) {

    actor->position = position;
    actor->node->getStore().addComponent(
        getEngine()->createComponent<SceneComponent>(actor->node, this->mesh));
  }
};
} // namespace Component

#endif