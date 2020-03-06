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


    class Mesh : public ComponentBase {

    public:

        std::vector<Vertex> vertices;
        std::vector<int> indices;

    };

	class PaintedMesh : public ComponentBase {
	public:
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;
		PaintedMesh(std::shared_ptr<Mesh>& mesh, std::shared_ptr<Material>& material) : mesh(mesh), material(material) {}
	};

	class MeshInstance : public ComponentBase {
	public:
		bool is_buffered = false;
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;
		std::vector<glm::mat4> instances{};
		MeshInstance(std::shared_ptr<Mesh>& mesh, std::shared_ptr<Material>& material) : mesh(mesh), material(material) {}
	};

	class MeshCollection : public ComponentBase {
	public:
		std::vector<std::shared_ptr<Mesh>> meshes;
	};


}


#endif //ENGINE_MESH_H
