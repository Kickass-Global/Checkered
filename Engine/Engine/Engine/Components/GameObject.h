//
// Created by root on 15/1/20.
//

#ifndef ENGINE_GAMEOBJECT_H
#define ENGINE_GAMEOBJECT_H

#include "ComponentId.h"
#include "ComponentBase.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Component {

    class GameObject : public ComponentBase<ClassId::GameObject> {
    public:
        glm::mat4 worldTransform;
    };

}


#endif //ENGINE_GAMEOBJECT_H
