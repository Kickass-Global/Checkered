//
// Created by Jackson Cougar Wiebe on 2/26/2020.
//

#include "material.hpp"
#include <Pipeline/Library.h>
Material::Material() : textures() {
  shader = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Program>(
      "Assets/Programs/basic.json");
}
