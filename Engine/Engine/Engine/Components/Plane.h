//
// Created by root on 17/1/20.
//

#ifndef ENGINE_PLANE_H
#define ENGINE_PLANE_H

#include "Mesh.h"

namespace Component {

    class Plane : public Component::Mesh {

        Plane() {
            vertices.emplace_back(aiVector3D{-0.5f, -0.5f, 0.0f},
                                  aiVector3D{0, 0, 0},
                                  aiVector3D{0, 0, 0});
            vertices.emplace_back(aiVector3D{0.5f, -0.5f, 0.0f},
                                  aiVector3D{0, 0, 0},
                                  aiVector3D{1.0f, 0, 0});
            vertices.emplace_back(aiVector3D{-0.5f, 0.5f, 0.0f},
                                  aiVector3D{0, 0, 0},
                                  aiVector3D{0, 1.0f, 0});
            vertices.emplace_back(aiVector3D{0.5f, 0.5f, 0.0f},
                                  aiVector3D{0, 0, 0},
                                  aiVector3D{1.0f, 1.0f, 0});
        }
        
    };

}

#endif //ENGINE_PLANE_H
