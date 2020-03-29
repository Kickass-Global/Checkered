//
// Created by Jackson Cougar Wiebe on 3/13/2020.
//

#ifndef ENGINE_BITMAP_HPP
#define ENGINE_BITMAP_HPP

#include <glad/glad.h>
#include <string>
#include <istream>
#include <SOIL2.h>

#include "Component.h"

namespace Component {

    // for use with HUDS and menus
    struct TextureArray {
        int width, height;
        GLuint m_texture_id;

        explicit TextureArray(const std::string &filename) : m_texture_id(0) { load(filename); }

        void load(const std::string &filename) {

            m_texture_id = SOIL_load_OGL_texture(
                filename.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
                SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
            );
            glBindTexture(GL_TEXTURE_2D, m_texture_id);

            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
            Engine::assertLog<module>(m_texture_id, "Loading texture '", filename, "'.");
        }
    };
}


#endif //ENGINE_BITMAP_HPP
