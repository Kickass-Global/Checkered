//
// Created by Jackson Cougar Wiebe on 4/6/2020.
//

#include "Wheel.hpp"

#include <Mesh.h>
#include <Pipeline/Library.h>

Wheel::Wheel() {
  auto wheel_mesh =
      getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
          "Assets/Meshes/wheel.obj");
  auto wheel_material = getEngine()->createComponent<Material>();
  wheel_material->textures.emplace_back(
      getEngine()->createComponent<Component::Texture>(
          "Assets/Textures/Vehicle_Car01_c.png"));
  mesh = getEngine()->createComponent<PaintedMesh>(wheel_mesh, wheel_material);
}

void Wheel::setTransform(const physx::PxTransform &T) {
  mesh->getStore().eraseComponentsOfType<WorldTransform>();
  mesh->getStore().emplaceComponent<WorldTransform>(T);
}
