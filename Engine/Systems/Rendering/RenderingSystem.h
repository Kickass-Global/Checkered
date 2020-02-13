//
// Created by root on 9/1/20.
//

#ifndef ENGINE_RENDERINGSYSTEM_H
#define ENGINE_RENDERINGSYSTEM_H

#include "glad/glad.h"
#include "../../Components/Component.h"
#include "../../Systems/systeminterface.hpp"
#include "RenderingBatch.h"
#include "Engine.h"

#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace Rendering {

    class RenderingSystem;

    class Shader {
        GLuint m_id;

    public:

        Shader();

        GLuint id() const;
        Shader(GLenum shader_type, std::vector<std::string> &lines);

        ~Shader();
    };

    class Program : public Component::ComponentBase<Component::ClassId::Program> {
        GLuint m_id;

    public:

        template <typename... Ts>
        explicit Program(std::vector<std::shared_ptr<Rendering::Shader>> shaders) {

            m_id = glCreateProgram();

            Engine::log<module>("Creating program ", m_id);

            for(auto&& shader : shaders) glAttachShader(m_id, shader->id());

            glLinkProgram(m_id);

            GLint successful_link;
            glGetProgramiv(m_id, GL_LINK_STATUS, &successful_link);

            if (successful_link != GL_TRUE) {
                GLint maxLength = 0;
                glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> infoLog(maxLength);
                glGetProgramInfoLog(m_id, maxLength, &maxLength, &infoLog[0]);
                Engine::log<module>(
                        std::string(infoLog.begin(), infoLog.end()));
            }
            Engine::assertLog<module>(successful_link != GL_FALSE, "Link shader program");

            for(auto&& shader : shaders) glDetachShader(m_id, shader->id());
        }

        GLuint programId();

        void bind();

        ~Program();
    };

    class RenderingSystem : public Engine::SystemInterface {

        GLFWwindow *window;

        std::vector<std::shared_ptr<Rendering::RenderBatch>> batches;

    public:

        static Component::ComponentEvent<int, int> onWindowSizeChanged;

        void initialize() override;

        static void windowSizeHandler(GLFWwindow* /*window*/, int width, int height);

        GLFWwindow* getWindow();

        void updateInstanceData(Component::ComponentId id, int size, float* data, int stride);

        void buffer(const Component::Mesh& data );

        void update(Engine::deltaTime /*elapsedTime*/) override;

        ~RenderingSystem();

        std::shared_ptr<Rendering::RenderBatch>
        findSuitableBufferFor(const std::shared_ptr<Component::Mesh> &data);

        std::shared_ptr<Rendering::RenderBatch> push_back(
            std::shared_ptr<Rendering::RenderBatch> batch);

    };
}

#endif //ENGINE_RENDERINGSYSTEM_H
