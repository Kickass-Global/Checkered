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
#include "../../Components/Component.h"
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

            if (!successful_link) {
                GLint maxLength = 0;
                glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> infoLog(maxLength);
                glGetProgramInfoLog(m_id, maxLength, &maxLength, &infoLog[0]);
                Engine::log<module>(std::string(infoLog.begin(), infoLog.end()));
            }
			Engine::assertLog<module>(successful_link != GL_FALSE, "Link shader program");

            for(auto&& shader : shaders) glDetachShader(m_id, shader->id());
        }

        GLuint programId();

        void bind();

        ~Program();
    };

    class RenderingSystem {

        GLFWwindow* window;

        std::vector<std::shared_ptr<Rendering::RenderBatch>> batches;

    public:

        Component::ComponentId onBillboardModifiedHandler;

        static Component::ComponentEvent<int, int> onWindowSizeChanged;

        void initialize();

        static void windowSizeHandler(GLFWwindow* /*window*/, int width, int height);

        void onBillboardModified(const Component::EventArgs<Component::ComponentId>& args)
        {
            auto& billboard = std::get<0>(args.values);
            Engine::log<module>("onBillboardModified=", billboard);
            buffer(*billboard.data<Component::Mesh>());
        }

        GLFWwindow* getWindow();

        void updateInstanceData(Component::ComponentId id, int size, float* data) {

            Engine::log<module>("Updating instance data of component#", id);

            auto it = std::find_if(batches.begin(), batches.end(),
                         [id](auto batch) { return batch->contains(id); });

			Engine::assertLog<module>(it != batches.end(), "check for valid batch");

            it->get()->update(id, 2, size, data);
        }

        void buffer(const Component::Mesh& data );

        void update(Engine::frametime /*elapsedTime*/);

        ~RenderingSystem();

        std::shared_ptr<Rendering::RenderBatch> findSuitableBufferFor(std::shared_ptr<Component::Mesh> data);

        std::shared_ptr<Rendering::RenderBatch> push_back(
            std::shared_ptr<Rendering::RenderBatch> batch);

    };
}

#endif //ENGINE_RENDERINGSYSTEM_H
