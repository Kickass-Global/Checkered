//
// Created by root on 12/1/20.
//

#ifndef ENGINE_MESH_H
#define ENGINE_MESH_H

#include <vector>
#include "glm/glm.hpp"

namespace Component {

    class Mesh {
    public:
        std::vector<glm::vec3> vertices;
        std::vector<int> indices;
    };

}


#endif //ENGINE_MESH_H
