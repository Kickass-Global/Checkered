//
// Created by root on 12/1/20.
//

#include "Mesh.h"

Component::Vertex::Vertex(aiVector3D position, aiVector3D normal, aiVector3D texcoord) :
        position(position.x, position.y, position.z),
        normal(normal.x, normal.y, normal.z),
        texcoord(texcoord.x, texcoord.y, texcoord.z) {}
