//
// Created by Jackson Cougar Wiebe on 3/13/2020.
//

#ifndef ENGINE_FONTSYSTEM_HPP
#define ENGINE_FONTSYSTEM_HPP

#include <ft2build.h>
#include <Pipeline/Library.h>
#include FT_FREETYPE_H
#include "RenderingSystem.h"
#include "text.hpp"

using Rendering::Program;
namespace Engine {
    /**
     * reference : https://learnopengl.com/code_viewer.php?code=in-practice/text_rendering
     */
    class FontSystem : public SystemInterface {
    public:
        std::shared_ptr<Program> program;


        void initialize() override {
            // FreeType
            FT_Library ft;
            // All functions return a value different than 0 whenever an error occurred
            if (FT_Init_FreeType(&ft))
                std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

            // Load font as face
            FT_Face face;
            if (FT_New_Face(ft, "Assets/Fonts/Troutbeck DEMO.otf", 0, &face))
                std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

            program = Pipeline::Library::getAsset<Program>(
                "Assets/Programs/font.json"
            );

            Engine::createComponent<Text>("This is just a test", 1, glm::vec3{0, 1, 0});

            glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(640), 0.0f, static_cast<GLfloat>(480));
            program->bind();
            glUniformMatrix4fv(
                glGetUniformLocation(program->programId(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            // Set size to load glyphs as
            FT_Set_Pixel_Sizes(face, 0, 48);

            // Disable byte-alignment restriction
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            // Load first 128 characters of ASCII set
            for (GLubyte c = 0; c < 128; c++) {
                // Load character glyph
                if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                    std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                    continue;
                }
                // Generate texture
                GLuint texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                glTexImage2D(
                    GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED,
                    GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer
                );
                // Set texture options
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // Now store character for later use
                Character character = {texture, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                                       glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                                       static_cast<GLuint>(face->glyph->advance.x)};
                Characters.insert(std::pair<GLchar, Character>(c, character));
            }
            glBindTexture(GL_TEXTURE_2D, 0);
            // Destroy FreeType once we're finished
            FT_Done_Face(face);
            FT_Done_FreeType(ft);


            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

        }

        void update(Engine::deltaTime time) override {
            // find all text that needs to be rendered during this frame
            for (auto text : Engine::getStore().getRoot().getComponentsOfType<Text>()) {
                RenderText(*program, text->text, 0, 0, 1, {0, 1, 0});
            }
        }

    private:
        /// Holds all state information relevant to a character as loaded using FreeType
        struct Character {
            GLuint TextureID;   // ID handle of the glyph texture
            glm::ivec2 Size;    // Size of glyph
            glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
            GLuint Advance;    // Horizontal offset to advance to next glyph
        };

        std::map<GLchar, Character> Characters;
        GLuint VAO, VBO;

        void RenderText(
            Program &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color
        ) {
            // Activate corresponding render state
            s.bind();
            glUniform3f(
                glGetUniformLocation(s.programId(), "textColor"), color.x, color.y, color.z
            );
            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(VAO);

            glDepthFunc(GL_ALWAYS);
            glEnable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Iterate through all characters
            std::string::const_iterator c;
            for (c = text.begin(); c != text.end(); c++) {
                Character ch = Characters[*c];

                GLfloat xpos = x + ch.Bearing.x * scale;
                GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

                GLfloat w = ch.Size.x * scale;
                GLfloat h = ch.Size.y * scale;
                // Update VBO for each character
                GLfloat vertices[6][4] = {{xpos,     ypos + h, 0.0, 0.0},
                                          {xpos,     ypos,     0.0, 1.0},
                                          {xpos + w, ypos,     1.0, 1.0},

                                          {xpos,     ypos + h, 0.0, 0.0},
                                          {xpos + w, ypos,     1.0, 1.0},
                                          {xpos + w, ypos + h, 1.0, 0.0}};
                // Render glyph texture over quad
                glBindTexture(GL_TEXTURE_2D, ch.TextureID);
                // Update content of VBO memory
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                // Render quad
                glDrawArrays(GL_TRIANGLES, 0, 6);
                // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
                x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
            }


            glDisable(GL_CULL_FACE);
            glDisable(GL_BLEND);
            glDepthFunc(GL_LESS);

            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    };
}


#endif //ENGINE_FONTSYSTEM_HPP
