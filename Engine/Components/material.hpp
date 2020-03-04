//
// Created by Jackson Cougar Wiebe on 2/26/2020.
//

#pragma once

#ifndef ENGINE_MATERIAL_HPP
#define ENGINE_MATERIAL_HPP

#include <vector>
#include <memory>

#include "Component.h"
#include "glad/glad.h"
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

		Material(std::shared_ptr<Program> shader) : shader(shader), textures() {}

        virtual void bind() {
            int index = 0;
            for (auto &texture : textures) {
                glActiveTexture(GL_TEXTURE0 + index++);
                glBindTexture(GL_TEXTURE_2D, texture->m_texture_id);
            }
        }
    };
}

#endif //ENGINE_MATERIAL_HPP
