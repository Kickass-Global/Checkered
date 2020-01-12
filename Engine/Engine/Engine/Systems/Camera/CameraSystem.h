//
// Created by root on 11/1/20.
//

#ifndef ENGINE_CAMERASYSTEM_H
#define ENGINE_CAMERASYSTEM_H

#include <map>
#include <algorithm>
#include <string>

#include "glm.hpp"
#include "gtc/quaternion.hpp"
#include "../../Components/ComponentId.h"
#include "../../Components/Camera.h"
#include "../../SystemCalls.h"

namespace Camera {

    constexpr char module[] = "Camera";

    class CameraSystem {

        std::map<Component::ComponentId, Component::Camera> cameras;

        void update(glm::vec3 position, glm::quat orientation) {

        }

    public:

        void onKeyPress(int key) {
            system_calls::log<module>("onKeyPress: ", key);
        }

    };

}

#endif //ENGINE_CAMERASYSTEM_H
