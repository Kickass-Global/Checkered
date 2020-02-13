//
// Created by root on 11/1/20.
//

#ifndef ENGINE_CAMERASYSTEM_H
#define ENGINE_CAMERASYSTEM_H

#include <map>
#include <algorithm>
#include <string>
#include <chrono>
#include <functional>
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#define _USE_MATH_DEFINES

#include <math.h>

#include "../../main.h"
#include "ComponentId.h"
#include "Camera.h"
#include "systeminterface.hpp"
#include "Index.h"
#include "Dirty.h"
#include "ComponentEvent.h"
#include "EventHandler.h"
#include "Engine.h"
#include "Events/Events.h"
#include "SceneComponent.h"

namespace Camera {

    namespace {
        char module[] = "Camera";
    }

    class CameraSystem : public Engine::SystemInterface {

    public:

        static Component::ComponentId id();

        Component::ComponentId onKeyPressHandler;
        Component::ComponentId onKeyDownHandler;
        Component::ComponentId onKeyUpHandler;
        Component::ComponentId onWindowSizeHandler;

		CameraSystem() {

			// create event handlers

			onKeyPressHandler = Engine::EventSystem::createHandler(this, &CameraSystem::onKeyPress);
			onKeyDownHandler = Engine::EventSystem::createHandler(this, &CameraSystem::onKeyDown);
			onKeyUpHandler = Engine::EventSystem::createHandler(this, &CameraSystem::onKeyUp);
			onWindowSizeHandler = Engine::EventSystem::createHandler(this, &CameraSystem::onWindowSizeChanged);

		}

        float x = 0;
        float y = 0;
        float u = 0;

        float sensitivity = 100.0f;
        float increment = static_cast<float>(M_PI) / 180.0f;

        std::set<int> keys;

        void update(Engine::deltaTime elapsed) override {
            for (auto &&key : keys) {
                if (!keys.count(key)) continue;

                switch (key) {
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
            

            for (auto&& camera : Component::Index::entitiesOf(Component::ClassId::Camera))
            {
                //if (std::abs(zoom) < 0.0001 && std::abs(x_rotation) < 0.0001 && std::abs(y_rotation) < 0.0001) continue;

                auto data = Component::Index::entityData<Component::Camera>(camera);

                auto delta = glm::quat(glm::vec3(
                    glm::degrees(x_rotation),
                    glm::degrees(y_rotation), 0));

                data->rotation *= delta;
                data->position.z += u;

                //if camera view matrix has changed mark it as dirty
                Component::Index::addComponent(camera, Component::Dirty::id());
                Engine::log<module, Engine::low>("Marking Camera ", camera, " dirty");

				// check if the camera is attached to a component, get that components 
				// transform and update the camera to lookat that object.
				auto sceneComponents = camera.childComponentsOfClass(Component::ClassId::SceneComponent);
				auto is_attached = !sceneComponents.empty();

				if (is_attached)
				{
					auto component = *sceneComponents.begin();

					Engine::log<module>("Updating camera to look at #", component);
					
					auto component_transform = component.data<Component::SceneComponent>()->getWorldTransform();
					data->position = component_transform[3] + component_transform[2] * 5.0f + glm::vec4{0, 2, 0, 0};
					glm::vec3 eye = data->position;
					glm::vec3 target = component_transform[3];
					glm::vec3 up = { 0,1,0 };
					data->view = glm::lookAt(eye, target, up);
				}

				// reset deltas
                x = 0;
                y = 0;
                u = 0;
            }

        }

        void onWindowSizeChanged(const Component::EventArgs<int,int>& args) {

            Engine::log<module, Engine::low>(
                    "onWindowSizeChanged=", std::get<0>(args.values), ", ", std::get<0>(args.values));

            auto&& width = std::get<0>(args.values);
            auto&& height = std::get<1>(args.values);

            for (auto&& camera : Component::Index::entitiesOf(Component::ClassId::Camera))
            {
                auto data = Component::Index::entityData<Component::Camera>(camera);
                data->viewport.width = width;
                data->viewport.height = height;

                Component::Index::addComponent(camera, Component::Dirty::id());
            }
        }

        void onKeyDown(const Component::EventArgs<int>& args) {
            Engine::log<module, Engine::low>("onKeyDown=", std::get<0>(args.values));
            keys.emplace(std::get<int>(args.values));
        }

        void onKeyUp(const Component::EventArgs<int>& args) {
            Engine::log<module, Engine::low>("onKeyUp=", std::get<0>(args.values));
            keys.erase(std::get<int>(args.values));
        }

        void onKeyPress(const Component::EventArgs<int>& args) {
            Engine::log<module, Engine::low>("onKeyPress=", std::get<0>(args.values));
        }

    };

}

#endif //ENGINE_CAMERASYSTEM_H
