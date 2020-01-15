//
// Created by root on 11/1/20.
//

#ifndef ENGINE_RENDERINGBATCH_H
#define ENGINE_RENDERINGBATCH_H

#include <vector>

#include "../../Components/ComponentId.h"
#include "../../Components/Shader.h"

#include "glad/glad.h"
#include "../../Components/Mesh.h"
#include <GLFW/glfw3.h>
#include <map>

namespace Rendering {
    class RenderingSystem;
    
    struct BufferDetails
    {
        int offset;
        int count;
    };
    
    class BatchBuffer {
        
        GLuint m_id;
        int m_size;
        int m_fill;
        int m_stride;
        int m_type;

    public:

        BatchBuffer(int bufferMaxSize, int stride, int type) : m_size(bufferMaxSize), m_fill(0), m_stride(stride), m_type(type) {
        
            glGenBuffers(1, &m_id);
            glBindBuffer(m_type, m_id);
            glBufferData(m_type, m_size, nullptr, GL_STATIC_DRAW);

        }

        template <typename T>
        BufferDetails push_back(std::vector<T> data) {
            
            glBindBuffer(m_type, m_id);
            glBufferSubData(m_type, m_fill, sizeof(T) * data.size(), data.data());

            auto offset = m_fill;
            m_fill += data.size() * sizeof(T);
        
            //assert(m_fill <= m_size, "Checking buffer fill");
            
            return {offset, static_cast<int>(data.size()) };
        }

        GLuint id() {
            return m_id;
        }

        size_t stride() {
            return m_stride;
        }

        size_t count() {
            return m_fill / m_stride;
        }
    };

    class RenderBatch {

        GLuint vao;

    public:

        Component::ComponentId shader;
        std::shared_ptr<Rendering::BatchBuffer> arrayBuffer;
        std::shared_ptr<Rendering::BatchBuffer> elementBuffer;

        std::map<Component::ComponentId, BufferDetails[2]> details;

        RenderBatch(std::shared_ptr<Rendering::BatchBuffer> arrayBuffer,
            std::shared_ptr<Rendering::BatchBuffer> elementBuffer);

        void push_back(const Component::Mesh& mesh)
        {
            details[mesh.id()][0] = arrayBuffer->push_back(mesh.vertices);
            details[mesh.id()][1] = elementBuffer->push_back(mesh.indices);
        }

        /**
         * Binds the VAO and sets up all resources needed to draw the geometry in the batch.
         */
        void bind(Rendering::RenderingSystem &renderingSystem);

        /**
         * Calls appropriate glDraw* command to draw the geometry in the batch.
         */
        void draw(Rendering::RenderingSystem &renderingSystem);

        bool contains(Component::ComponentId id) const
        {
            return details.count(id) > 0;
        }

        void remove(Component::ComponentId id)
        {
            details.erase(id);
        }

        void assign_shader(Component::Shader shader);
    };
}
#endif //ENGINE_RENDERINGBATCH_H
