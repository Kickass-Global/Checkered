//
// Created by Jackson Cougar Wiebe on 2/26/2020.
//

#ifndef ENGINE_MATERIAL_HPP
#define ENGINE_MATERIAL_HPP

#include <vector>
#include "Component.h"
#include "glad/glad.h"
#include "texture.hpp"

namespace Component {

    class Material : public Component::ComponentBase<Component::ClassId::Material> {
    public:
        Component::ComponentId shader;
        std::vector<Component::ComponentId> textures;
        // maybe some uniforms down here, you know, whatever feels right...

		Material(const ComponentId shader_id) : shader(shader_id), textures() {}

        void bind() {
            int index = 0;
            for (auto &&texture : textures) {
                glActiveTexture(GL_TEXTURE0 + index++);
                glBindTexture(GL_TEXTURE_2D, texture.data<Texture>()->m_texture_id);
            }
        }
    };

}

#endif //ENGINE_MATERIAL_HPP
