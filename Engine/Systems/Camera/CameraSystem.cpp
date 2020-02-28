//
// Created by root on 11/1/20.
//

#include "CameraSystem.h"

void ::Camera::CameraSystem::update(Engine::deltaTime elapsed) {
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


    for (auto &&camera : Component::Index::entitiesOf<Component::Camera>()) {
        //if (std::abs(zoom) < 0.0001 && std::abs(x_rotation) < 0.0001 && std::abs(y_rotation) < 0.0001) continue;

        auto data = Component::Index::entityData<Component::Camera>(camera);

        auto delta = glm::quat(glm::vec3(0, glm::degrees(x_rotation), 0));

        data->rotation *= delta;
        data->position.z += u;

        //if camera view matrix has changed mark it as dirty
        camera.addTag<Component::Dirty>();
        Engine::log<module, Engine::low>("Marking Camera ", camera, " dirty");

        // check if the camera is attached to a component, get that components
        // transform and update the camera to look at that object.

        auto meta = data->target.data<Component::Vehicle>();
        if (data->target && meta->pxVehicle) {

            const auto &component = data->target;
            Engine::log<module>("Updating camera to look at #", component);

            auto offset = glm::toMat4(data->rotation) * glm::vec4(data->offset, 1);

            // this line of code controls the camera returning to 'neutral' position behind the target...
            data->rotation = glm::lerp(data->rotation, meta->rotation, 0.05f);
            data->position = meta->position + glm::vec3(offset);

            glm::vec3 eye = data->position;
            glm::vec3 target = meta->position;
            glm::vec3 world_up = {0, 1, 0};

            data->view = glm::lookAt(eye, target, world_up);
        }
        // reset deltas
        x = 0;
        y = 0;
        u = 0;
    }

}

void ::Camera::CameraSystem::onWindowSizeChanged(const Component::EventArgs<int, int> &args) {

    Engine::log<module, Engine::low>(
            "onWindowSizeChanged=", std::get<0>(args.values), ", ", std::get<0>(args.values));

    auto &&width = std::get<0>(args.values);
    auto &&height = std::get<1>(args.values);

    for (auto &&camera : Component::Index::entitiesOf<Component::Camera>()) {
        auto data = Component::Index::entityData<Component::Camera>(camera);
        data->viewport.width = width;
        data->viewport.height = height;

        camera.addTag<Component::Dirty>();
    }
}

void ::Camera::CameraSystem::onKeyPress(const Component::EventArgs<int> &args) {
    Engine::log<module, Engine::low>("onKeyPress=", std::get<0>(args.values));
}

void ::Camera::CameraSystem::onKeyUp(const Component::EventArgs<int> &args) {
    Engine::log<module, Engine::low>("onKeyUp=", std::get<0>(args.values));
    keys.erase(std::get<int>(args.values));
}

void ::Camera::CameraSystem::onKeyDown(const Component::EventArgs<int> &args) {
    Engine::log<module, Engine::low>("onKeyDown=", std::get<0>(args.values));
    keys.emplace(std::get<int>(args.values));
}
