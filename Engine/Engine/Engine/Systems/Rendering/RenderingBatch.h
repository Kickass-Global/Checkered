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
        void push_back(T* data, int size) {
            
            glBindBuffer(m_type, m_id);
            glBufferSubData(m_type, m_fill, size, data);
            m_fill += size;
        
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

        RenderBatch(std::shared_ptr<Rendering::BatchBuffer> arrayBuffer,
            std::shared_ptr<Rendering::BatchBuffer> elementBuffer);


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
