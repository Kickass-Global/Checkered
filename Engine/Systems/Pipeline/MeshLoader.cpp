#include "MeshLoader.h"
#include "Library.h"
#include <Engine.h>
#include <scenery.cpp>

std::shared_ptr<Component::Mesh>
Pipeline::MeshLoader::load(std::string filename) {

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		filename, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

	if (!scene) {
		Engine::log<module, Engine::high>(importer.GetErrorString());
	}
	Engine::assertLog<module>(scene, "Loading resource " + filename);

	auto&& data = scene->mMeshes[0];

	Engine::assertLog<module>(data, "Loading mesh data");

	auto mesh = getEngine()->createComponent<Component::Mesh>();

	// todo: error checking & sanity

	for (auto i = 0u; i < data->mNumVertices; ++i) {
		mesh->vertices.emplace_back(data->mVertices[i], data->mNormals[i],
			data->mTextureCoords[0][i]);
	}

	for (auto i = 0u; i < data->mNumFaces; ++i) {
		auto&& face = data->mFaces[i];
		Engine::assertLog<module>(face.mNumIndices == 3,
			"Mesh face is triangulated");

		for (auto j = 0u; j < face.mNumIndices; ++j) {
			mesh->indices.push_back(face.mIndices[j]);
		}
	}

	return mesh;
}

std::shared_ptr<Component::Mesh> Pipeline::MeshLoader::load(aiMesh* data) {

	Engine::assertLog<module>(data, "Loading mesh data");

	auto mesh = getEngine()->createComponent<Component::Mesh>();

	// todo: error checking & sanity

	for (auto i = 0u; i < data->mNumVertices; ++i) {
		mesh->vertices.emplace_back(data->mVertices[i], data->mNormals[i],
			data->mTextureCoords[0][i]);
	}

	for (auto i = 0u; i < data->mNumFaces; ++i) {
		auto&& face = data->mFaces[i];
		Engine::assertLog<module>(face.mNumIndices == 3,
			"Mesh face is triangulated");

		for (auto j = 0u; j < face.mNumIndices; ++j) {
			mesh->indices.push_back(face.mIndices[j]);
		}
	}

	return mesh;
}

std::ostream& operator<<(std::ostream& out, const aiMetadataEntry& entry) {
	switch (entry.mType) {
	case AI_AISTRING:
		return out << ((aiString*)entry.mData)->C_Str();
	default:
		return out << entry.mData;
	}
}

void Pipeline::ScenarioLoader::load_scenario(const ScenarioInterface& scenario,
	std::string filename) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate);

	if (!scene) {
		Engine::log<module, Engine::high>(importer.GetErrorString());
	}
	Engine::assertLog<module>(scene, "Loading resource " + filename);

	for (auto i = 0; i < scene->mRootNode->mNumChildren; i++) {
		Engine::log<module, high>(
			"Node '", scene->mRootNode->mChildren[i]->mName.C_Str(), "'");
	}

	for (auto i = 0; i < scene->mMetaData->mNumProperties; i++) {
		Engine::log<module, high>("Meta '", scene->mMetaData->mKeys[i].C_Str(),
			"' = ", scene->mMetaData->mValues[i].mData);
	}

	for (auto i = 0; i < scene->mNumMeshes; i++) {
		Engine::log<module, high>("Mesh '", scene->mMeshes[i]->mName.C_Str(), "'");

		// load and buffer all the contained meshes into the engine because why
		// not...
		auto mesh =
			getEngine()->getSubSystem<class MeshLoader>()->load(scene->mMeshes[i]);
		getEngine()->getSubSystem<Pipeline::Library>()->setAsset(
			scene->mMeshes[i]->mName.C_Str(), mesh);
	}

	auto static_collection =
		std::find_if(scene->mRootNode->mChildren,
			scene->mRootNode->mChildren + scene->mRootNode->mNumChildren,
			[](auto node) { return node->mName == aiString("Static"); });

	if (static_collection !=
		scene->mRootNode->mChildren + scene->mRootNode->mNumChildren) {
		for (int i = 0; i < (*static_collection)->mNumChildren; ++i) {
			// load all static object as scenery then...
			auto child = (*static_collection)->mChildren[i];
			auto mesh_index = child->mMeshes[0];
			auto transform = scene->mRootNode->mTransformation * (*static_collection)->mTransformation * child->mTransformation;
			auto mesh_name = scene->mMeshes[mesh_index]->mName.C_Str();
			auto instance_mesh = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(mesh_name);
			auto basic_shader_program =
				getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Program>(
					"Assets/Programs/basic.json");
			auto material = getEngine()->createComponent<Component::Material>(basic_shader_program);

			Instance<Scenery> building_instances;
			glm::mat4 T(transform.a1, transform.b1, transform.c1, transform.d1,
				transform.a2, transform.b2, transform.c2, transform.d2,
				transform.a3, transform.b3, transform.c3, transform.d3,
				transform.a4, transform.b4, transform.c4, transform.d4);
			building_instances.add_instance_at(T, instance_mesh, material);
		}
	}
}
