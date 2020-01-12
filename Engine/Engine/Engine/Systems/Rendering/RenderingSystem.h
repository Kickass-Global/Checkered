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
#include "../../SystemCalls.h"
#include "../../Components/Shader.h"
#include "../../Components/Camera.h"
#include "RenderingBatch.h"
#include "../../Components/Index.h"
#include "../../Components/Dirty.h"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Rendering {

    constexpr char module[] = "RenderingSystem";
    constexpr auto assert = system_calls::assert<module>;

    class RenderingSystem;


    class Shader {
        GLuint m_id;

    public:

        Shader() : m_id(-1) {}

        GLuint id() const { return m_id; }
        Shader(GLenum shader_type, std::vector<std::string> &lines) {

            m_id = glCreateShader(shader_type);
            system_calls::log<module>("Creating shader ", m_id);

            std::vector<const GLchar *> cstrings;
            std::vector<int> lengths;

            for (auto &&line : lines) {
                cstrings.push_back(line.c_str());
                lengths.push_back(line.size());
            }

            glShaderSource(m_id, lines.size(), cstrings.data(), lengths.data());
            glCompileShader(m_id);

            GLint success;
            glGetShaderiv(m_id, GL_COMPILE_STATUS, &success);

            if(!success) {
                GLint maxLength = 0;
                glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(m_id, maxLength, &maxLength, &infoLog[0]);

                system_calls::log<module>(std::string(infoLog.begin(), infoLog.end()));
            }
            assert(success != GL_FALSE, "shader creation");
        }

        ~Shader() {
            system_calls::log<module>("Deleting shader ", m_id);
            glDeleteShader(m_id);
        }
    };

    class Program {
        GLuint m_id;

        template <typename T>
        void attach(T& shader)
        {
            glAttachShader(m_id, shader.id());
        }
        template <typename T, typename... Ts>

        void attach(T& shader, Ts&... shaders)
        {
            glAttachShader(m_id, shader.id());
            return attach(shaders...);
        }

        template <typename T>
        void detach(T& shader)
        {
            glDetachShader(m_id, shader.id());
        }
        template <typename T, typename... Ts>

        void detach(T& shader, Ts&... shaders)
        {
            glDetachShader(m_id, shader.id());
            return detach(shaders...);
        }
    public:

        template <typename... Ts>
        explicit Program(std::vector<std::unique_ptr<Rendering::Shader>> shaders) {

            m_id = glCreateProgram();

            system_calls::log<module>("Creating program ", m_id);

            for(auto&& shader : shaders) glAttachShader(m_id, shader->id());

            glLinkProgram(m_id);

            GLint successful_link;
            glGetProgramiv(m_id, GL_LINK_STATUS, &successful_link);

            if (!successful_link) {
                GLint maxLength = 0;
                glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> infoLog(maxLength);
                glGetProgramInfoLog(m_id, maxLength, &maxLength, &infoLog[0]);
                system_calls::log<module>(std::string(infoLog.begin(), infoLog.end()));
            }
            assert(successful_link != GL_FALSE, "Link shader program");

            for(auto&& shader : shaders) glDetachShader(m_id, shader->id());
        }

        GLuint id() {
            return m_id;
        }

        void bind() {
//            system_calls::log<module>("Binding program ", id);
            glUseProgram(m_id);
        }

        ~Program() {
            system_calls::log<module>("Deleting program ", m_id);
            glDeleteProgram(m_id);
        }
    };

    class RenderingSystem {

        GLFWwindow* window;

        std::vector<Rendering::RenderBatch> batches;
        std::map<Component::ComponentId, std::unique_ptr<Rendering::Program>> shader_programs;

    public:

        void bind_shader(Component::ComponentId id)
        {
            auto it = shader_programs.find(id);
            assert(it != shader_programs.end(), "Binding shader");

            it->second->bind();
        }

        void push_back(Component::ComponentId id, std::unique_ptr<Rendering::Program> program) {
            shader_programs.emplace(id, std::move(program));
        }

        void initialize() {

            assert(glfwInit(), "initialize GLFW");

            window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);

            glfwMakeContextCurrent(window);

            assert(gladLoadGLLoader((GLADloadproc) glfwGetProcAddress), "initialize GLAD");
        }

        GLFWwindow* getWindow() {
            return window;
        }

        void update() {
            glClear(GL_COLOR_BUFFER_BIT);

            for (auto &&batch : batches) {
                // if batch should be drawn
                {
                    auto program = shader_programs[batch.shader]->id();
                    for(auto&& camera : Component::Index::entitiesOf(Component::Index::Type::Camera))
                    {
                        auto is_dirty = Component::Index::hasComponent(camera, Component::Dirty::id());
                        if(is_dirty)
                        {
                            auto data = Component::Index::entityData<Component::Camera>(camera);
                            auto view_matrix = glm::toMat4(data->rotation);

                            glUseProgram(program);
                            glUniformMatrix4fv(glGetUniformLocation(program, "View"),
                                    1, false, glm::value_ptr(view_matrix));

//                            system_calls::log<module>("Camera ", camera, " is dirty");
                            // update program uniforms, etc.
                            Component::Index::removeComponent(camera, Component::Dirty::id());
                        }
                    }

                    // bind programs, textures, and uniforms needed to render the batch
                    batch.bind(*this);
                    batch.draw(*this);
                }
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        ~RenderingSystem() {
            glfwDestroyWindow(window);
        }

        void push_back(RenderBatch &batch) {
            batches.push_back(batch);
        }

    };
}

#endif //ENGINE_RENDERINGSYSTEM_H
