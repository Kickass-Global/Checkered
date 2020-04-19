//
// Created by Jackson Cougar Wiebe on 2/26/2020.
//

#pragma once

#ifndef ENGINE_MATERIAL_HPP
#define ENGINE_MATERIAL_HPP

#include "glad/glad.h"

#include <memory>
#include <utility>
#include <vector>

#include "Component.h"
#include "texture.hpp"

namespace Rendering {
  class Program;
}

using Rendering::Program;

namespace Component {

  class Material : public ComponentBase {
public:
    std::shared_ptr<Program> shader;
    std::vector<std::shared_ptr<Texture>> textures;

    // maybe some uniforms down here, you know, whatever feels right...

    explicit Material(std::shared_ptr<Program> shader) : shader(std::move(shader)), textures() {}

    Material();

    explicit Material(std::string texture_asset_name) : Material() {
      textures.push_back(getEngine()->createComponent<Component::Texture>(texture_asset_name));
    }

    virtual void bind() {

      //    glEnable(GL_BLEND);
      //    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      int index = 0;
      for (auto &texture : textures) {
        if (texture) {
          glActiveTexture(GL_TEXTURE1 + index++);
          glBindTexture(GL_TEXTURE_2D, texture->m_texture_id);
        } else {
          glActiveTexture(GL_TEXTURE1 + index++);
          glBindTexture(GL_TEXTURE_2D, 0);
        }
      }
    }
  };
}// namespace Component

#endif// ENGINE_MATERIAL_HPP
