//
// Created by Jackson Cougar Wiebe on 2/26/2020.
//

#pragma once

#ifndef ENGINE_MATERIAL_HPP
#define ENGINE_MATERIAL_HPP

#include "glad/glad.h"

#include <utility>
#include <vector>
#include <memory>

#include "texture.hpp"
#include "Component.h"

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

        virtual void bind() {

            int index = 0;
            for (auto &texture : textures) {
                if (texture) {
                    glActiveTexture(GL_TEXTURE1 + index++);
                    glBindTexture(GL_TEXTURE_2D, texture->m_texture_id);
                }
                else
                {
                    glActiveTexture(GL_TEXTURE1 + index++);
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
            }
        }
    };
}

#endif //ENGINE_MATERIAL_HPP
