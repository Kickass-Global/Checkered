#ifndef MESHLOADER_H
#define MESHLOADER_H

#include <fstream>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "ShaderLoader.h"
#include "../../Components/Mesh.h"

namespace Pipeline {

	class MeshLoader {
	public:
		std::shared_ptr<Component::Mesh> load(std::string filename)
		{
			Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile( filename,
                                                      aiProcess_CalcTangentSpace       |
                                                      aiProcess_Triangulate            |
                                                      aiProcess_JoinIdenticalVertices  |
                                                      aiProcess_SortByPType);

            // If the import failed, report it
            if(!scene)
            {
                Engine::log<module, Engine::Importance::high>( importer.GetErrorString());
                assert(scene, "Loading resource " + filename);
            }

            Component::Mesh mesh;
            mesh.vertices.assign((glm::vec3*)scene->mMeshes[0]->mVertices,
                                 (glm::vec3*)scene->mMeshes[0]->mVertices + scene->mMeshes[0]->mNumVertices);


            auto&& data = scene->mMeshes[0];
            for (auto i = 0; i < data->mNumFaces; ++i)
            {
                auto&& face = data->mFaces[i];
                for (auto j = 0; j < face.mNumIndices; ++j)
                    mesh.indices.push_back(face.mIndices[j]);
            }

            return std::make_shared<Component::Mesh>(mesh);
		}
	};

}

#endif // !MESHLOADER_H
