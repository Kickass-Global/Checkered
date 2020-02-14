//
// Created by root on 11/1/20.
//

#include "RenderingBatch.h"
#include "RenderingSystem.h"
#include "../../Components/ComponentId.h"

namespace Rendering {

    void RenderBatch::assign_shader(Component::Shader shader) {
        this->shader = shader.id();
    }

    void RenderBatch::draw(Rendering::RenderingSystem &renderingSystem) {

        for(auto [key, detail] : details) {

            Engine::log<module>("Drawing #", key);
			// todo pass in the stride somehow
			glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, detail[1].count, GL_UNSIGNED_INT, 0, detail[2].count, detail[1].offset, detail[2].offset / 64);
        }
    }

    void RenderBatch::bind(Rendering::RenderingSystem &renderingSystem) {

        Engine::log<module>("Binding shader#", shader);
        shader.data<Program>()->bind();
        glBindVertexArray(vao);
    }


    RenderBatch::RenderBatch(std::shared_ptr<Rendering::BatchBuffer> arrayBuffer,
                             std::shared_ptr<Rendering::BatchBuffer> elementBuffer,
                             std::shared_ptr<Rendering::BatchBuffer> instanceBuffer)
        : arrayBuffer(arrayBuffer), elementBuffer(elementBuffer), instanceBuffer(instanceBuffer) {

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

        glBindVertexBuffer(1, instanceBuffer->id(), 0, instanceBuffer->stride());
        glVertexBindingDivisor(1, 1);

        // setup instance matrix attribute buffer

        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);
        glEnableVertexAttribArray(6);
        glEnableVertexAttribArray(7);

        glVertexAttribFormat(4, 4, GL_FLOAT, false, 0);
        glVertexAttribFormat(5, 4, GL_FLOAT, false, 1 * sizeof(glm::vec4));
        glVertexAttribFormat(6, 4, GL_FLOAT, false, 2 * sizeof(glm::vec4));
        glVertexAttribFormat(7, 4, GL_FLOAT, false, 3 * sizeof(glm::vec4));

        glVertexAttribBinding(4, 1);
        glVertexAttribBinding(5, 1);
        glVertexAttribBinding(6, 1);
        glVertexAttribBinding(7, 1);

        glBindVertexArray(0);
    }

    void RenderBatch::push_back(const Component::Mesh &mesh) {
        details[mesh.id()][0] = arrayBuffer->push_back(mesh.vertices);
        details[mesh.id()][1] = elementBuffer->push_back(mesh.indices);
        details[mesh.id()][2] = instanceBuffer->push_back(std::vector<glm::mat4>{glm::mat4(1)});
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
        glBufferData(m_type, m_size, nullptr, GL_DYNAMIC_DRAW);

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