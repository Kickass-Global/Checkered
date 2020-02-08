#ifndef MESHLOADER_H
#define MESHLOADER_H

#include <fstream>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "ShaderLoader.h"
#include "../../Components/Mesh.h"
#include "Library.h"

namespace Pipeline {

    class MeshLoader {
    public:

        static Component::Mesh *load(std::string filename) {

            if (Library::contains(filename)) {
                Engine::log<module>("Using resource from library ", filename);
                return Library::at(filename).data<Component::Mesh>();
            }

            Assimp::Importer importer;
            const aiScene *scene = importer.ReadFile(filename,
                                                     aiProcess_CalcTangentSpace |
                                                     aiProcess_Triangulate |
                                                     aiProcess_JoinIdenticalVertices |
                                                     aiProcess_SortByPType);

            if (!scene) {
                Engine::log<module, Engine::Importance::high>( importer.GetErrorString());
            }
			Engine::assertLog<module>(scene, "Loading resource " + filename);

            auto&& data = scene->mMeshes[0];
			Engine::assertLog<module>(data, "Loading mesh data");

            Component::Mesh mesh = Component::Mesh();

            // todo: error checking & sanity

            for (auto i = 0u; i < data->mNumVertices; ++i)
            {
                mesh.vertices.emplace_back(
                    data->mVertices[i],
                    data->mNormals[i],
                    data->mTextureCoords[0][i]
                );
            }

            for (auto i = 0u; i < data->mNumFaces; ++i)
            {
                int x = 000;
                auto&& face = data->mFaces[i];
				Engine::assertLog<module>(face.mNumIndices == 3, "Mesh face is triangulated");

                for (auto j = 0u; j < face.mNumIndices; ++j) {
                    mesh.indices.push_back(face.mIndices[j]);
                }
            }

            auto shaders = Component::Index::entitiesOf(
                    Component::ClassId::Shader);
            if (shaders.size() > 0) {
                mesh.shader = *(shaders.begin());
            }

            Library::emplace(filename, mesh.id());
            return Engine::addComponent(std::make_unique<Component::Mesh>(mesh));
        }
	};

}

#endif // !MESHLOADER_H
