#ifndef MESHLOADER_H
#define MESHLOADER_H

#include <Mesh.h>
#include <memory>
#include <string>

namespace Pipeline {
namespace {
extern char module[];
}
class ScenarioLoader : public SystemInterface {
public:
  void load_scenario(const ScenarioInterface &scenario, std::string filename) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(
        filename, aiProcess_Triangulate);

    if (!scene) {
      Engine::log<module, Engine::high>(importer.GetErrorString());
    }
    Engine::assertLog<module>(scene, "Loading resource " + filename);
    for (auto i = 0; i < scene->mRootNode->mNumChildren; i++)
    {
        Engine::log<module, high>("Node '", scene->mRootNode->mChildren[i]->mName.C_Str(), "'");
    }
    
  }
};
class MeshLoader : public SystemInterface {
public:
  std::shared_ptr<Component::Mesh> load(std::string filename);
};

} // namespace Pipeline

#endif // !MESHLOADER_H
