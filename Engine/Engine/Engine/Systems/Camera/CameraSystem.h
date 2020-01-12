//
// Created by root on 11/1/20.
//

#ifndef ENGINE_CAMERASYSTEM_H
#define ENGINE_CAMERASYSTEM_H

#include <map>
#include <algorithm>
#include <string>
#include <chrono>
#include <GLFW/glfw3.h>
#define _USE_MATH_DEFINES

#include <math.h>

#include "../../Engine.h"
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
        float u = 0;

        float sensitivity = 50.0f;
        float increment = M_PI / 180.0f;

        std::set<int> keys;

        void update(frametime elapsed) {
            for (auto&& key : keys)
            {
                if (!keys.count(key)) continue;

                switch (key)
                {
                case GLFW_KEY_RIGHT:
                    x += increment;
                    break;
                case GLFW_KEY_DOWN:
                    y -= increment;
                    break;
                case GLFW_KEY_LEFT:
                    x -= increment;
                    break;
                case GLFW_KEY_UP:
                    y += increment;
                    break;
                case GLFW_KEY_PAGE_UP:
                    u += increment;
                    break;
                case GLFW_KEY_PAGE_DOWN:
                    u -= increment;
                    break;
                }
            }

            float x_rotation = x / sensitivity * elapsed;
            float y_rotation = y / sensitivity * elapsed;
            float zoom = u / sensitivity * elapsed;
            

            for (auto&& camera : Component::Index::entitiesOf(Component::Index::Type::Camera))
            {
                //if (x_rotation == 0.0 && y_rotation == 0.0) continue;

                auto data = Component::Index::entityData<Component::Camera>(camera);

                auto delta = glm::quat(glm::vec3(
                    glm::degrees(x_rotation),
                    glm::degrees(y_rotation), 0));

                data->rotation *= delta;
                data->position.z += u;

                //if camera view matrix has changed mark it as dirty
                Component::Index::addComponent(camera, Component::Dirty::id());
                Engine::log<module, Engine::Importance::low>("Marking Camera ", camera, " dirty");
                
                x = 0;
                y = 0;
                u = 0;
            }

        }

        void onWindowSizeChanged(int width, int height) {

            for (auto&& camera : Component::Index::entitiesOf(Component::Index::Type::Camera))
            {
                auto data = Component::Index::entityData<Component::Camera>(camera);
                data->viewport.width = width;
                data->viewport.height = height;

                Component::Index::addComponent(camera, Component::Dirty::id());
            }
        }

        void onKeyDown(int key) {
            keys.emplace(key);
        }

        void onKeyUp(int key) {
            keys.erase(key);
        }

        void onKeyPress(int key) {
        }

    };

}

#endif //ENGINE_CAMERASYSTEM_H
