//
// Created by root on 11/1/20.
//

#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H

#include <vector>
#include "ComponentId.h"

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "ComponentBase.h"

namespace Component {

    class Camera : public ComponentBase<ClassId::Camera> {

    public:

        struct Viewport {
            int x, y, width, height;
        } viewport;

        glm::vec3 position;
        glm::quat rotation;

        Camera();
    };

}

#endif //ENGINE_CAMERA_H
