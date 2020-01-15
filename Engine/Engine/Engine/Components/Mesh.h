//
// Created by root on 12/1/20.
//

#ifndef ENGINE_MESH_H
#define ENGINE_MESH_H

#include <vector>
#include "glm/glm.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace Component {

    class Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 texcoord;
    
    public:

        Vertex(aiVector3D position, aiVector3D normal, aiVector3D texcoord) :
            position(position.x, position.y, position.z), 
            normal(normal.x, normal.y, normal.z), 
            texcoord(texcoord.x, texcoord.y, texcoord.z) {}
    };

    class Mesh {

    public:

        std::vector<Vertex> vertices;
        std::vector<int> indices;

    };

}


#endif //ENGINE_MESH_H
