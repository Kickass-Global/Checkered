//
// Created by root on 11/1/20.
//

#ifndef ENGINE_CAMERASYSTEM_H
#define ENGINE_CAMERASYSTEM_H

#include <map>
#include <algorithm>
#include <string>
#include <GLFW/glfw3.h>

#include "../../Components/ComponentId.h"
#include "../../Components/Camera.h"
#include "../../SystemCalls.h"
#include "../../Components/Index.h"
#include "../../Components/Dirty.h"

namespace Camera {

    constexpr char module[] = "Camera";

    class CameraSystem {

    public:

        float x = 0;
        float y = 0;

        void update() {

            float x_rotation = x;
            float y_rotation = y;

            for(auto&& camera : Component::Index::entitiesOf(Component::Index::Type::Camera))
            {
                if(x_rotation == 0.0 && y_rotation == 0.0) continue;

                auto data = Component::Index::entityData<Component::Camera>(camera);
                
                auto delta = glm::quat(glm::vec3(
                    glm::degrees(x_rotation),
                    glm::degrees(y_rotation), 0));
                
                data->rotation *= delta;

                //if camera view matrix has changed mark it as dirty
                Component::Index::addComponent(camera, Component::Dirty::id());
//                system_calls::log<module>("Marking Camera ", camera, " dirty");
x = 0;
y = 0;
            }

        }

        void onKeyPress(int key) {

            system_calls::log<module>("onKeyPress: ", key);

            if (key == GLFW_KEY_RIGHT) {
                x += 0.1f;
            } else if (key == GLFW_KEY_LEFT) {
                x -= 0.1f;
            }

            if (key == GLFW_KEY_UP) {
                y += 0.1f;
            } else if (key == GLFW_KEY_DOWN) {
                y -= 0.1f;
            }

        }

    };

}

#endif //ENGINE_CAMERASYSTEM_H
