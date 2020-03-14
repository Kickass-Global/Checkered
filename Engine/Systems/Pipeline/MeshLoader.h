#ifndef MESHLOADER_H
#define MESHLOADER_H

#include <memory>
#include <string>
#include <Mesh.h>

namespace Pipeline {
    namespace {
        extern char module[];
    }
    class MeshLoader : public SystemInterface {
    public:

        std::shared_ptr<Component::Mesh> load(std::string filename);

    };

}

#endif // !MESHLOADER_H
