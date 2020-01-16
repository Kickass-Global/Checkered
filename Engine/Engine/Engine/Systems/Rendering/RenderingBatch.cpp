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
        for(auto [key, detail] : details) {
            glDrawElementsBaseVertex(GL_TRIANGLES, detail[1].count, GL_UNSIGNED_INT, 0, detail[1].offset);
        }
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

    void RenderBatch::push_back(const Component::Mesh &mesh) {
        details[mesh.id()][0] = arrayBuffer->push_back(mesh.vertices);
        details[mesh.id()][1] = elementBuffer->push_back(mesh.indices);
    }

    bool RenderBatch::contains(Component::ComponentId id) const {
        return details.count(id) > 0;
    }

    void RenderBatch::remove(Component::ComponentId id) {
        details.erase(id);
    }

    BatchBuffer::BatchBuffer(int bufferMaxSize, int stride, int type)
    : m_size(bufferMaxSize), m_fill(0), m_stride(stride), m_type(type) {

        glGenBuffers(1, &m_id);
        glBindBuffer(m_type, m_id);
        glBufferData(m_type, m_size, nullptr, GL_STATIC_DRAW);

    }

    GLuint BatchBuffer::id() {
        return m_id;
    }

    size_t BatchBuffer::stride() {
        return m_stride;
    }

    size_t BatchBuffer::count() {
        return m_fill / m_stride;
    }
}