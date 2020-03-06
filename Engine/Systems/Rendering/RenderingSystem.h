//
// Created by root on 9/1/20.
//

#pragma once

#ifndef ENGINE_RENDERINGSYSTEM_H
#define ENGINE_RENDERINGSYSTEM_H

#include "glad/glad.h"
#include "RenderingBatch.h"
#include "Engine.h"

#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace Rendering {

	class RenderingSystem;

	class Shader : public ComponentBase {
		GLuint m_id;

	public:

		Shader();

		GLuint glid() const;

		Shader(GLenum shader_type, std::vector<std::string> &lines);

		~Shader();
	};

	class Program : public ComponentBase {

		GLuint m_id;

	public:

		template<typename... Ts>
		explicit Program(std::vector<std::shared_ptr<Rendering::Shader>> shaders) {

			m_id = glCreateProgram();

			Engine::log<module>("Creating program ", m_id);

			for (auto &&shader : shaders) glAttachShader(m_id, shader->glid());

			glLinkProgram(m_id);

			GLint successful_link;
			glGetProgramiv(m_id, GL_LINK_STATUS, &successful_link);

			if (successful_link != GL_TRUE) {
				GLint maxLength = 0;
				glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetProgramInfoLog(m_id, maxLength, &maxLength, &infoLog[0]);
				Engine::log<module, Engine::high>(std::string(infoLog.begin(), infoLog.end()));
			}
			Engine::assertLog<module>(successful_link != GL_FALSE, "Link shader program");

			for (auto &&shader : shaders) glDetachShader(m_id, shader->glid());
		}

		GLuint programId();

		void bind();

		~Program();
	};

	class RenderingSystem : public Engine::SystemInterface {

		GLFWwindow *window;

		std::vector<std::shared_ptr<Rendering::GeometryBatch>> batches;

	public:

		static Component::EventDelegate<int, int> onWindowSizeChanged;

		void initialize() override;

		static void windowSizeHandler(GLFWwindow * /*window*/, int width, int height);

		GLFWwindow *getWindow();

		void updateInstanceData(std::shared_ptr<Mesh>& mesh_id, std::shared_ptr<Material>& material_id, int size, float *data, int stride);

		void buffer(std::shared_ptr<Mesh>& mesh, std::shared_ptr<Material>& material);

		void update(Engine::deltaTime /*elapsedTime*/) override;

		~RenderingSystem();

		std::shared_ptr<Rendering::GeometryBatch>
			findSuitableBufferFor(std::shared_ptr<Mesh>& data, std::shared_ptr<Material>& material);

		std::shared_ptr<Rendering::GeometryBatch> push_back(
			const std::shared_ptr<Rendering::GeometryBatch> &batch
		);

	};
}

#endif //ENGINE_RENDERINGSYSTEM_H
