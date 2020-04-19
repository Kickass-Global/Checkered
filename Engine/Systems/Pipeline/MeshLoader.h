#ifndef MESHLOADER_H
#define MESHLOADER_H

#include <Mesh.h>
#include <memory>
#include <string>
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

namespace Pipeline {
namespace {
extern char module[];
}
class ScenarioLoader : public SystemInterface {
  public:
  template<typename T>
  void load_scenario(const ScenarioInterface &scenario, std::string filename);
};
class MeshLoader : public SystemInterface {
public:
  std::shared_ptr<Component::Mesh> load(std::string filename);
  std::shared_ptr<Component::Mesh> load(aiMesh *data);
};

} // namespace Pipeline


template<typename T>
void Pipeline::ScenarioLoader::load_scenario(const ScenarioInterface &scenario,
                                             std::string filename) {
  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(filename, aiProcess_Triangulate);

  if (!scene) { Engine::log<module, Engine::high>(importer.GetErrorString()); }
  Engine::assertLog<module>(scene, "Loading resource " + filename);

  for (auto i = 0; i < scene->mRootNode->mNumChildren; i++) {
    Engine::log<module, high>("Node '", scene->mRootNode->mChildren[i]->mName.C_Str(), "'");
  }

  for (auto i = 0; i < scene->mMetaData->mNumProperties; i++) {
    Engine::log<module, high>("Meta '", scene->mMetaData->mKeys[i].C_Str(),
                              "' = ", scene->mMetaData->mValues[i].mData);
  }

  for (auto i = 0; i < scene->mNumMeshes; i++) {
    Engine::log<module, high>("Mesh '", scene->mMeshes[i]->mName.C_Str(), "'");

    // load and buffer all the contained meshes into the engine because why
    // not...
    auto mesh = getEngine()->getSubSystem<class MeshLoader>()->load(scene->mMeshes[i]);
    getEngine()->getSubSystem<Pipeline::Library>()->setAsset(scene->mMeshes[i]->mName.C_Str(),
                                                             mesh);
  }

  for (int i = 0; i < scene->mRootNode->mNumChildren; ++i) {
    // load all static object as scenery then...
    auto child = scene->mRootNode->mChildren[i];
    auto mesh_index = child->mMeshes[0];
    auto transform = child->mTransformation;
    auto mesh_name = scene->mMeshes[mesh_index]->mName.C_Str();
    auto instance_mesh = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(mesh_name);

    auto material = getEngine()->createComponent<Component::Material>();
    material->textures.push_back(
        getEngine()->createComponent<Component::Texture>("Assets/Textures/props.png"));

    Instance<T> building_instances;
    glm::mat4 T(transform.a1, transform.b1, transform.c1, transform.d1, transform.a2, transform.b2,
                transform.c2, transform.d2, transform.a3, transform.b3, transform.c3, transform.d3,
                transform.a4, transform.b4, transform.c4, transform.d4);

    building_instances.add_instance_at(T, instance_mesh, material);
  }
}


#endif // !MESHLOADER_H
