//
// Created by root on 11/1/20.
//

#ifndef ENGINE_RENDERINGBATCH_H
#define ENGINE_RENDERINGBATCH_H

#include "../../Components/ComponentId.h"
#include "../../Components/Shader.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

namespace Rendering {
    class RenderingSystem;

    class RenderBatch {

        GLuint vao;
        GLuint vertex_data_buffer;
        int size;
        int fill;
        int stride = 12;

        Component::ComponentId shader;

    public:

        RenderBatch(int bufferMaxSize);

        void push_back(void *data, int size);

        /**
         * Binds the VAO and sets up all resources needed to draw the geometry in the batch.
         */
        void bind(Rendering::RenderingSystem &renderingSystem);

        /**
         * Calls appropriate glDraw* command to draw the geometry in the batch.
         */
        void draw(Rendering::RenderingSystem &renderingSystem);

        void assign_shader(Component::Shader shader);
    };
}
#endif //ENGINE_RENDERINGBATCH_H
