//
// Created by root on 11/1/20.
//

#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H

#include <vector>
#include "ComponentId.h"

namespace Component {

    class Camera {
        std::vector<Component::ComponentId> components;
    };
}

#endif //ENGINE_CAMERA_H
