//
// Created by root on 19/1/20.
//

#include "Billboard.h"
#include <Mesh.h>
#include <Pipeline/Library.h>

Billboard::Billboard(std::shared_ptr<Texture> sprite) {
    auto material = getEngine()->createComponent<Material>(
        getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Program>(
            "Assets/Programs/billboard.json"
        ));
    material->textures.push_back(sprite);
    mesh_instance = getEngine()->createComponent<PaintedMesh>(
        getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
            "Assets/Meshes/billboard_quad.obj"
        ), material
    );
}
