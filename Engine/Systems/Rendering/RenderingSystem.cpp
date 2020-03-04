//
// Created by root on 9/1/20.
//

#include <sstream>
#include <Billboard.h>
#include "RenderingSystem.h"

#include "tags.h"
#include "Camera.h"
#include "SceneComponent.h"
#include "Engine.h"
#include "WorldTransform.h"

Component::EventDelegate<int, int> Rendering::RenderingSystem::onWindowSizeChanged("onWindowSizeChanged");

namespace {
	const char module[] = "RenderingSystem";
}

void Rendering::RenderingSystem::update(Engine::deltaTime time) {

	std::stringstream ss;
	ss << "frametime: " << time << "ms" << std::endl;
	auto title = ss.str();
	glfwSetWindowTitle(window, title.c_str());

	// Find and update any GameObjects with meshes that should be drawn...
	auto meshes = Engine::getStore().getRoot().getComponentsOfType<MeshInstance>();
	for (const auto& instance : meshes) {
		if (!instance->is_buffered)
		{
			instance->is_buffered = true;
			Engine::log<module, Engine::high>("Updating batch data of#", instance->getId());
			buffer(instance->mesh, instance->material);
		}
		if (instance->instances.size() > 0)
		{
			Engine::log<module, Engine::low>("Updating instances(", instance->instances.size(), ") of component#", instance);
			std::vector<glm::mat4> transform_data;

			for (auto &&transform : instance->instances) {
				transform_data.push_back(transform.world_matrix);
			}

			Engine::assertLog(!transform_data.empty(), "Checking that instance data is not empty.");

			updateInstanceData(
				instance->mesh,
				instance->material,
				static_cast<int>(sizeof(glm::mat4) * transform_data.size()),
				(float *)transform_data.data(),
				sizeof(glm::mat4)
			);

		}
		instance->instances.clear();
	}


	glClearColor(0, 0, 0.5f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto &&batch : batches) {
		for (auto &camera : Engine::getStore().getRoot().getComponentsOfType<Component::Camera>()) {

			if (camera->is_dirty) {

				auto view_matrix = camera->view;
				auto world_matrix = glm::translate(camera->position);

				auto perspective_matrix = glm::perspective(
					45.0f,
					static_cast<float>(camera->viewport.width) /
					camera->viewport.height,
					0.1f,
					1000.0f
				);

				glViewport(0, 0, camera->viewport.width, camera->viewport.height);

				batch->shader->bind();

				glUniformMatrix4fv(
					glGetUniformLocation(
						batch->shader->programId(),
						"M_View"
					),
					1, false, glm::value_ptr(view_matrix));

				glUniformMatrix4fv(
					glGetUniformLocation(
						batch->shader->programId(),
						"M_Perspective"
					),
					1, false, glm::value_ptr(perspective_matrix));


			}
		}
	}

	// this code handles drawing billboards into the world (hud, sprites, etc).
	for (auto &camera : Engine::getStore().getRoot().getComponentsOfType<Component::Camera>()) {
		for (const auto &sprite : Engine::getStore().getRoot().getComponentsOfType<Component::Billboard>()) {

			const auto &camera_view_matrix = camera->view;
			const auto &viewport = camera->viewport;

			auto offset = glm::translate(
				glm::vec3(
					sprite->plot.x / viewport.width - sprite->anchor.x,
					sprite->plot.y / viewport.height - sprite->anchor.y,
					0.0f
				)
			);

			auto anchor = glm::translate(glm::vec3(sprite->anchor.x, sprite->anchor.y, 0.0f));

			auto scale = glm::scale(
				glm::vec3(
					sprite->plot.width / viewport.width,
					sprite->plot.height / viewport.height,
					sprite->plot.height / viewport.height // todo, hack
				)
			);


			sprite->mesh_instance->instances.push_back(WorldTransform(offset * scale * anchor));
		}

		break; // only use the first camera...
	}

	for (auto &&batch : batches) {
		if (!batch->details.empty()) {
			// bind programs, textures, and uniforms needed to render the batch
			batch->bind(*this);
			batch->draw(*this);
		}
	}

	glfwSwapBuffers(window);
	glfwPollEvents();
}

Rendering::RenderingSystem::~RenderingSystem() {
	glfwDestroyWindow(window);
}

std::shared_ptr<Rendering::GeometryBatch>
Rendering::RenderingSystem::findSuitableBufferFor(
	std::shared_ptr<Mesh>& mesh, std::shared_ptr<Material>& material
) {

	auto arrayBuffer = std::make_shared<Rendering::BatchBuffer>(
		10000000,
		sizeof(mesh->vertices[0]),
		GL_ARRAY_BUFFER
		);

	auto elementBuffer = std::make_shared<Rendering::BatchBuffer>(
		10000000,
		sizeof(mesh->indices[0]),
		GL_ELEMENT_ARRAY_BUFFER
		);

	auto instanceBuffer = std::make_shared<Rendering::BatchBuffer>(
		10000000,
		sizeof(glm::mat4),
		GL_ARRAY_BUFFER
		);

	auto batch = std::make_shared<GeometryBatch>(arrayBuffer, elementBuffer, instanceBuffer);
	batch->shader = material->shader;


	return push_back(batch);
}

std::shared_ptr<Rendering::GeometryBatch> Rendering::RenderingSystem::push_back(
	const std::shared_ptr<Rendering::GeometryBatch> &batch
) {
	batches.push_back(batch);
	return batches.back();
}

void Rendering::RenderingSystem::buffer(std::shared_ptr<Mesh>&mesh, std::shared_ptr<Material>& material) {

	// if the data is already buffered we want to update the existing buffer data

	auto match = std::find_if(
		batches.begin(), batches.end(),
		[mesh, material](const std::shared_ptr<GeometryBatch> &batch) {
		return batch->contains(mesh, material);
	}
	);

	if (match != batches.end()) {
		// mark/remove buffered data so its not used
		Engine::log<module>("Removing #", mesh->id, " from batch#", (*match)->vao);
		(*match)->remove(mesh, material);
	}

	// find a batch that can hold the data, or make one
	auto batch = findSuitableBufferFor(mesh, material);

	// todo: if the buffered data size matches that in the buffer we should actually replace it here...
	batch->push_back(mesh, material);
}

GLFWwindow *Rendering::RenderingSystem::getWindow() {
	return window;
}

void Rendering::RenderingSystem::windowSizeHandler(GLFWwindow *, int width, int height) {
	onWindowSizeChanged(width, height);
}

void Rendering::RenderingSystem::initialize() {

	Engine::assertLog<module>(glfwInit(), "initialize GLFW");

	window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);

	glfwSetWindowSizeCallback(window, windowSizeHandler);

	glfwMakeContextCurrent(window);

	Engine::assertLog<module>(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "initialize GLAD");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_CULL_FACE);

}

void Rendering::RenderingSystem::updateInstanceData(std::shared_ptr<Mesh>& mesh, std::shared_ptr<Material>& material, int size, float *data, int stride) {

	Engine::log<module, Engine::low>("Updating instance data of component#", mesh);

	auto it = std::find_if(
		batches.begin(), batches.end(),
		[mesh, material](const auto batch)
	{
		return batch->contains(mesh, material);
	}
	);

	Engine::assertLog<module>(it != batches.end(), "check for valid batch");

	it->get()->update(mesh, material, 2, size, data, stride);
}

Rendering::Program::~Program() {
	Engine::log<module>("Deleting program ", m_id);
	glDeleteProgram(m_id);
}

void Rendering::Program::bind() {
	Engine::log<module, Engine::low>("Binding program ", m_id);
	glUseProgram(m_id);
}

GLuint Rendering::Program::programId() {
	return m_id;
}

Rendering::Shader::~Shader() {
	Engine::log<module>("Deleting shader ", m_id);
	glDeleteShader(m_id);
}

Rendering::Shader::Shader(GLenum shader_type, std::vector<std::string> &lines) {

	m_id = glCreateShader(shader_type);
	Engine::log<module>("Creating shader ", m_id);

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

	if (!success) {
		GLint maxLength = 0;
		glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(m_id, maxLength, &maxLength, &infoLog[0]);

		Engine::log<module, Engine::high>(std::string(infoLog.begin(), infoLog.end()));
	}
	Engine::assertLog<module>(success != GL_FALSE, "shader creation");
}

GLuint Rendering::Shader::glid() const { return m_id; }

Rendering::Shader::Shader() : m_id(static_cast<GLuint>(-1)) {}
