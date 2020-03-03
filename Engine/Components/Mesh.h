//
// Created by root on 12/1/20.
//

#pragma once

#ifndef ENGINE_MESH_H
#define ENGINE_MESH_H

#include <vector>
#include "glm/glm.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "ComponentId.h"
#include "ComponentBase.h"
#include "material.hpp"

namespace Component {

    struct Vertex {

        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 texcoord;

    public:
        Vertex(aiVector3D position, aiVector3D normal, aiVector3D texcoord);
    };

    class Mesh : public ComponentBase<ClassId::Mesh> {

    public:

        [[deprecated]]Component::ComponentId shader;
		[[deprecated]]Component::ComponentId material;
        std::vector<Vertex> vertices;
        std::vector<int> indices;

    };

	class MeshInstance : public ComponentBase<ClassId::MeshInstance> {
	public:
		bool is_buffered = false;
		ComponentId mesh;
		ComponentId material;
		std::vector<WorldTransform> instances{};
		MeshInstance(const Mesh &mesh, const Material& material) : mesh(mesh.id()), material(material.id()) {}
		MeshInstance(const ComponentId mesh_id, const ComponentId material_id) : mesh(mesh_id), material(material_id) {}
	};

}


#endif //ENGINE_MESH_H
