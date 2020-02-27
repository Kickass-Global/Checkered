//
// Created by Jackson Cougar Wiebe on 2/26/2020.
//

#ifndef ENGINE_TEXTURE_HPP
#define ENGINE_TEXTURE_HPP

#include <istream>
#include <glad/glad.h>
#include <SOIL2.h>
#include "Component.h"

namespace Component {
    class Texture : public Component::ComponentBase<Component::ClassId::Texture> {
    public:
        GLuint m_texture_id;
        explicit Texture(const std::string &filename) : m_texture_id(0) { load(filename); }
        void load(const std::string &filename) {
            m_texture_id = SOIL_load_OGL_texture
                    (
                            filename.c_str(),
                            SOIL_LOAD_AUTO,
                            SOIL_CREATE_NEW_ID,
                            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
                    );
            Engine::assertLog<module>(m_texture_id, "Loading texture '", filename, "'.");
        }
    };
}

#endif //ENGINE_TEXTURE_HPP
