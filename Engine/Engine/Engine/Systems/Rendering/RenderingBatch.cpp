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
        glDrawElements(GL_TRIANGLES, elementBuffer->count(), GL_UNSIGNED_INT, 0);
    }

    void RenderBatch::bind(Rendering::RenderingSystem &renderingSystem) {
        renderingSystem.bind_shader(this->shader);
        glBindVertexArray(vao);
    }


    RenderBatch::RenderBatch(std::shared_ptr<Rendering::BatchBuffer> arrayBuffer,
            std::shared_ptr<Rendering::BatchBuffer> elementBuffer) 
        : arrayBuffer(arrayBuffer), elementBuffer(elementBuffer) {
        glGenVertexArrays(1, &vao);

        glBindVertexArray(vao);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer->id());

        glBindBuffer(GL_ARRAY_BUFFER, arrayBuffer->id());

        glBindVertexBuffer(0, arrayBuffer->id(), 0, arrayBuffer->stride());
        
        glEnableVertexAttribArray(0);
        glVertexAttribFormat(0, 3, GL_FLOAT, false, 0);
        glVertexAttribBinding(0, 0);

        glEnableVertexAttribArray(1);
        glVertexAttribFormat(1, 3, GL_FLOAT, false, 1 * sizeof(glm::vec3));
        glVertexAttribBinding(1, 0);

        glEnableVertexAttribArray(2);
        glVertexAttribFormat(2, 3, GL_FLOAT, false, 2 * sizeof(glm::vec3));
        glVertexAttribBinding(2, 0);


        glBindVertexArray(0);
    }
}