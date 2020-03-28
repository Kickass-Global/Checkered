//
// Created by root on 19/1/20.
//

#include "Billboard.h"
#include <Mesh.h>
#include "Component.h"
#include "texture.hpp"
#include "material.hpp"
#include <Pipeline/Library.h>

Billboard::Billboard(const std::shared_ptr<Texture> &billboard_image) {
    auto material = getEngine()->createComponent<Component::Material>(
        getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Program>(
            "Assets/Programs/billboard.json"
        ));
    material->textures.push_back(billboard_image);
    auto billboard_mesh = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
        "Assets/Meshes/billboard_quad.obj"
    );
    mesh_instance = getEngine()->createNamedComponent<PaintedMesh>(
        "billboard_mesh_instance", billboard_mesh, material
    );
}
