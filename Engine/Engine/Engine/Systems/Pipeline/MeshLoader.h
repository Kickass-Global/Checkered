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

            if(!scene)
            {
                Engine::log<module, Engine::Importance::high>( importer.GetErrorString());
            }
            assert(scene, "Loading resource " + filename);

            auto&& data = scene->mMeshes[0];
            assert(data, "Loading mesh data");

            Component::Mesh mesh;

            // todo: error checking & sanity

            for (auto i = 0; i < data->mNumVertices; ++i)
            {
                mesh.vertices.emplace_back(
                    data->mVertices[i],
                    data->mNormals[i],
                    data->mTextureCoords[0][i]
                );
            }

            for (auto i = 0; i < data->mNumFaces; ++i)
            {
                auto&& face = data->mFaces[i];
                assert(face.mNumIndices == 3, "Mesh face is triangulated");

                for (auto j = 0; j < face.mNumIndices; ++j)
                {
                    mesh.indices.push_back(face.mIndices[j]);
                }
            }

            return std::make_shared<Component::Mesh>(mesh);
		}
	};

}

#endif // !MESHLOADER_H
