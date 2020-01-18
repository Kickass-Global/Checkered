//
// Created by root on 9/1/20.
//

#ifndef ENGINE_RENDERINGSYSTEM_H
#define ENGINE_RENDERINGSYSTEM_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <map>
#include "../../main.h"
#include "../../SystemCalls.h"
#include "../../Components/Shader.h"
#include "../../Components/Camera.h"
#include "RenderingBatch.h"
#include "../../Components/Index.h"
#include "../../Components/Dirty.h"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../../Components/Mesh.h"
#include "Rendering.h"
#include "../../Components/ComponentEvent.h"
#include "../../Engine.h"

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

    class Program {
        GLuint m_id;

    public:

        template <typename... Ts>
        explicit Program(std::vector<std::unique_ptr<Rendering::Shader>> shaders) {

            m_id = glCreateProgram();

            Engine::log<module>("Creating program ", m_id);

            for(auto&& shader : shaders) glAttachShader(m_id, shader->id());

            glLinkProgram(m_id);

            GLint successful_link;
            glGetProgramiv(m_id, GL_LINK_STATUS, &successful_link);

            if (!successful_link) {
                GLint maxLength = 0;
                glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> infoLog(maxLength);
                glGetProgramInfoLog(m_id, maxLength, &maxLength, &infoLog[0]);
                Engine::log<module>(std::string(infoLog.begin(), infoLog.end()));
            }
            assertLog(successful_link != GL_FALSE, "Link shader program");

            for(auto&& shader : shaders) glDetachShader(m_id, shader->id());
        }

        GLuint id();

        void bind();

        ~Program();
    };

    class RenderingSystem {

        GLFWwindow* window;

        std::vector<std::shared_ptr<Rendering::RenderBatch>> batches;
        std::map<Component::ComponentId, std::unique_ptr<Rendering::Program>> shader_programs;

    public:

        static Component::ComponentEvent<int, int> onWindowSizeChanged;

        void bind_shader(Component::ComponentId id);

        void push_back(Component::ComponentId id, std::unique_ptr<Rendering::Program> program);

        void initialize();

        static void windowSizeHandler(GLFWwindow* /*window*/, int width, int height);

        GLFWwindow* getWindow();

        void buffer(const Component::Mesh& data );

        void update(Engine::frametime /*elapsedTime*/);

        ~RenderingSystem();

        std::shared_ptr<Rendering::RenderBatch> findSuitableBufferFor(std::shared_ptr<Component::Mesh> data);

        std::shared_ptr<Rendering::RenderBatch> push_back(
            std::shared_ptr<Rendering::RenderBatch> batch);

    };
}

#endif //ENGINE_RENDERINGSYSTEM_H
