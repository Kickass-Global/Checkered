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

struct Camera : public ComponentBase<ClassId::Camera> {
        Component::ComponentId m_id;

        struct Viewport {
            int x, y, width, height;
        } viewport;

        glm::vec3 position;
        glm::quat rotation;

        Camera() : m_id(true), position(),
        rotation(1, 0, 0, 0),
        viewport { 0, 0, 640, 480 } {  }
        Component::ComponentId id() { return m_id; }
    };

}

#endif //ENGINE_CAMERA_H
