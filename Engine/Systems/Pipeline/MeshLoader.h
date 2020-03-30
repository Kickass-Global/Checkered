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
  void load_scenario(const ScenarioInterface &scenario, std::string filename);
};
class MeshLoader : public SystemInterface {
public:
  std::shared_ptr<Component::Mesh> load(std::string filename);
  std::shared_ptr<Component::Mesh> load(aiMesh *data);
};

} // namespace Pipeline

#endif // !MESHLOADER_H
