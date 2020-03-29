//
// Created by root on 19/1/20.
//

#include "Billboard.h"
#include "Component.h"
#include "material.hpp"
#include "texture.hpp"
#include <Mesh.h>
#include <Pipeline/Library.h>

Billboard::Billboard(const std::shared_ptr<Texture> &billboard_image) {
  auto material = getEngine()->createComponent<Component::Material>(
      getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Program>(
          "Assets/Programs/billboard.json"));
  material->textures.push_back(billboard_image);
  auto billboard_mesh =
      getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
          "Assets/Meshes/billboard_quad.obj");
  mesh_instance = getEngine()->createNamedComponent<PaintedMesh>(
      "billboard_mesh_instance", billboard_mesh, material);
}
Billboard::Billboard(const std::shared_ptr<PaintedMesh> &billboard_mesh) {
  mesh_instance = billboard_mesh;
}
