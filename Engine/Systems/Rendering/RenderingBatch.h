//
// Created by root on 11/1/20.
//

#pragma once

#ifndef ENGINE_RENDERINGBATCH_H
#define ENGINE_RENDERINGBATCH_H

#include <map>
#include <vector>

#include "../../Components/ComponentId.h"
#include "../../Components/Mesh.h"
#include "../../Components/Shader.h"
#include "material.hpp"
#include "Rendering.h"

#include "glad/glad.h"
#include "Engine.h"
#include <GLFW/glfw3.h>

using Component::Material;
using Component::Mesh;
using Component::ComponentId;

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

			Engine::log<module>("Pushing data into batch#", id());

			glBindBuffer(m_type, m_id);
			glBufferSubData(m_type, m_fill, sizeof(T) * data.size(), data.data());

			auto offset = m_fill;
			m_fill += data.size() * sizeof(T);

			Engine::assertLog<module>(m_fill < m_size, "Checking buffer fill");

			return { offset, static_cast<int>(data.size()), sizeof(T) };
		}

		template<typename T>
		BatchDescription push_back(int size, T *data, int stride) {

			Engine::log<module>("Pushing data into batch#", id());

			glBindBuffer(m_type, m_id);
			glBufferSubData(m_type, m_fill, stride * size, data);

			auto offset = m_fill;
			m_fill += size * stride;

			Engine::assertLog<module>(m_fill < m_size, "Checking buffer fill");

			return { offset, size, stride };
		}

		void replace_existing_data(int size, float *data, BatchDescription details) {
			Engine::log<module, Engine::low>("Replacing data in batch#", id());
			glBindBuffer(m_type, m_id);
			glBufferSubData(m_type, details.offset, size, data);
		}

		GLuint id(); 
		GLuint type();

		size_t stride();

		size_t count();
	};

	class GeometryBatch {

	public:

		std::shared_ptr<Program> shader;

		std::shared_ptr<Rendering::BatchBuffer> arrayBuffer;
		std::shared_ptr<Rendering::BatchBuffer> elementBuffer;
		std::shared_ptr<Rendering::BatchBuffer> instanceBuffer;

		std::map<std::pair<std::shared_ptr<Mesh>, std::shared_ptr<Material>>, BatchDescription[3]> details;

		GeometryBatch(std::shared_ptr<Rendering::BatchBuffer> arrayBuffer,
			std::shared_ptr<Rendering::BatchBuffer> elementBuffer,
			std::shared_ptr<Rendering::BatchBuffer> instanceBuffer
		);

		void push_back(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

		void update(std::shared_ptr<Mesh>& mesh,
			std::shared_ptr<Material>& material,
			int buffer,
			int size,
			float *data,
			int stride
		) {
			auto &detail = details.at(std::make_pair(mesh, material));
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

		bool contains(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) const;

		void remove(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

		~GeometryBatch() {
			Engine::log<module>("Deleting batch#", vao);
		}

		GLuint vao;
	};
}
#endif //ENGINE_RENDERINGBATCH_H
