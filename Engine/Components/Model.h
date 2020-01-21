//
// Created by root on 17/1/20.
//

#ifndef ENGINE_MODEL_H
#define ENGINE_MODEL_H


#include <vector>
#include "Shader.h"

namespace Component {

    /**
     * A model is a collection of meshes and shaders
     */
    class Model {
        std::vector<ComponentId> shaders;
        std::vector<ComponentId> meshes;
    };

}


#endif //ENGINE_MODEL_H
