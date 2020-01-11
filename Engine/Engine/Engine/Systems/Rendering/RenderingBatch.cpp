//
// Created by root on 11/1/20.
//

#include "RenderingBatch.h"
#include "RenderingSystem.h"

namespace Rendering {

    void RenderBatch::assign_shader(Component::Shader shader) {
        this->shader = shader.id();
    }

    void RenderBatch::draw(Rendering::RenderingSystem &renderingSystem) {
        glDrawArrays(GL_TRIANGLES, 0, fill / stride);
    }

    void RenderBatch::bind(Rendering::RenderingSystem &renderingSystem) {
        renderingSystem.bind_shader(this->shader);
        glBindVertexArray(vao);
    }

    void RenderBatch::push_back(void *data, int size) {
        glBindBuffer(GL_ARRAY_BUFFER, vertex_data_buffer);
        glBufferSubData(GL_ARRAY_BUFFER, fill, size, data);
        fill += size;
    }

    RenderBatch::RenderBatch(int bufferMaxSize) : size(bufferMaxSize), fill(0) {
        glGenBuffers(1, &vertex_data_buffer);
        glGenVertexArrays(1, &vao);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vertex_data_buffer);
        glBufferData(GL_ARRAY_BUFFER, bufferMaxSize, nullptr, GL_STATIC_DRAW);

        glBindVertexBuffer(0, vertex_data_buffer, 0, sizeof(float) * 3);

        glEnableVertexAttribArray(0);
        glVertexAttribFormat(0, 3, GL_FLOAT, false, 0);
        glVertexAttribBinding(0, 0);

        glBindVertexArray(0);
    }
}