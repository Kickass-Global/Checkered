//
// Created by root on 11/1/20.
//

#ifndef ENGINE_RENDERINGBATCH_H
#define ENGINE_RENDERINGBATCH_H

#include <map>
#include <vector>

#include "../../Components/ComponentId.h"
#include "../../Components/Mesh.h"
#include "../../Components/Shader.h"
#include "Rendering.h"

#include "glad/glad.h"
#include "Engine.h"
#include <GLFW/glfw3.h>

namespace Rendering {

	enum BatchBufferType {
		ArrayBuffer,
		ElementBuffer,
		InstanceBuffer
	};

	class RenderingSystem;

	/** describes a contiguous section of a buffer that can be drawn with a single draw* command. */
	struct BatchDescription {
		int offset; // start of element data (from beginning of buffer)
		int count; // how many elements
		int stride; // size of each element
	};

	class BatchBuffer {

		GLuint m_id;
		int m_size;
		int m_fill;
		int m_stride;
		int m_type;

	public:

		BatchBuffer(int bufferMaxSize, int stride, int type);

		template<typename T>
		BatchDescription push_back(std::vector<T> data) {

			Engine::log<module, Engine::low>("Pushing data into batch#", id());

			glBindBuffer(m_type, m_id);
			glBufferSubData(m_type, m_fill, sizeof(T) * data.size(), data.data());

			auto offset = m_fill;
			m_fill += data.size() * sizeof(T);

			Engine::assertLog<module>(m_fill <= m_size, "Checking buffer fill");

			return { offset, static_cast<int>(data.size()), sizeof(T) };
		}

		template<typename T>
		BatchDescription push_back(int size, T *data, int stride) {

			Engine::log<module, Engine::low>("Pushing data into batch#", id());

			glBindBuffer(m_type, m_id);
			glBufferSubData(m_type, m_fill, sizeof(T) * size, data);

			auto offset = m_fill;
			m_fill += size * stride;

			Engine::assertLog<module>(m_fill <= m_size, "Checking buffer fill");

			return { offset, size, stride };
		}

		void replace_existing_data(int size, float *data, BatchDescription details) {
			Engine::log<module, Engine::low>("Replacing data in batch#", id());
			glBindBuffer(m_type, m_id);
			glBufferSubData(m_type, details.offset, size, data);
		}

		GLuint id();

		size_t stride();

		size_t count();
	};

	class RenderBatch {

		GLuint vao;

	public:

		Component::ComponentId shader;

		std::shared_ptr<Rendering::BatchBuffer> arrayBuffer;
		std::shared_ptr<Rendering::BatchBuffer> elementBuffer;
		std::shared_ptr<Rendering::BatchBuffer> instanceBuffer;

		std::map<std::pair<ComponentId, ComponentId>, BatchDescription[3]> details;

		RenderBatch(std::shared_ptr<Rendering::BatchBuffer> arrayBuffer,
			std::shared_ptr<Rendering::BatchBuffer> elementBuffer,
			std::shared_ptr<Rendering::BatchBuffer> instanceBuffer);

		void push_back(const Component::Mesh &mesh, const Component::Material material);

		void update(const Component::ComponentId mesh_id, const ComponentId material_id, int buffer, int size, float *data, int stride) {
			Engine::log<module, Engine::low>("Updating component#", mesh_id);

			auto &detail = details.at(std::make_pair(mesh_id, material_id));
			int count = size / stride;
			auto replace_existing_data = count <= detail[2].count;

			switch (buffer) {
			case 2:
				if (replace_existing_data) {
					instanceBuffer->replace_existing_data(size, data, detail[buffer]);
				}
				else {
					detail[2] = instanceBuffer->push_back(count, data, stride);
				}
				break;
			}
		}

		/**
		 * Binds the VAO and sets up all resources needed to draw the geometry in the batch.
		 */
		void bind(Rendering::RenderingSystem &renderingSystem);

		/**
		 * Calls appropriate glDraw* command to draw the geometry in the batch.
		 */
		void draw(Rendering::RenderingSystem &renderingSystem);

		bool contains(Component::ComponentId mesh_id, Component::ComponentId material_id) const;

		void remove(Component::ComponentId id, Component::ComponentId material_id);

	};
}
#endif //ENGINE_RENDERINGBATCH_H
