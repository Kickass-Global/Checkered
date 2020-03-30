//
// Created by Jackson Cougar Wiebe on 2/26/2020.
//

#ifndef ENGINE_TEXTURE_HPP
#define ENGINE_TEXTURE_HPP

#include <glad/glad.h>
#include <istream>
#include "Component.h"
#include <SOIL2.h>

namespace Component {
    class Texture : public ComponentBase {
    public:
        GLuint m_texture_id;
        int width;
        int height;
        explicit Texture(const std::string &filename) : m_texture_id(0) { load(filename); }
        void load(const std::string &filename) {
            m_texture_id = SOIL_load_OGL_texture
                    (
                            filename.c_str(),
                            SOIL_LOAD_AUTO,
                            SOIL_CREATE_NEW_ID,
                            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
                    );

            glBindTexture(GL_TEXTURE_2D, m_texture_id);

            //Get Width, Height and Data of Image
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
            Engine::assertLog<module>(m_texture_id, "Loading texture '", filename, "'.");
        }
    };
}

#endif //ENGINE_TEXTURE_HPP
