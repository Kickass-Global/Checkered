//
// Created by root on 19/1/20.
//

#include <Mesh.h>
#include <Pipeline/Library.h>
#include <Camera.h>
#include "BillboardSystem.h"
#include <WorldTransform.h>
#include <glm/gtx/transform.hpp>


void Engine::BillboardSystem::update(Engine::deltaTime time) {

    // this code handles drawing billboards into the world (hud, sprites, etc).

    for (auto &camera :   getEngine()->getSubSystem<EngineStore>()->getRoot().getComponentsOfType<Component::Camera>()) {
        for (const auto &sprite :   getEngine()->getSubSystem<EngineStore>()->getRoot().getComponentsOfType<Component::Billboard>()) {

            const auto &camera_view_matrix = camera->view;
            const auto &viewport = camera->viewport;

            auto offset = glm::translate(
                glm::vec3(
                    sprite->plot.x / viewport.width - sprite->anchor.x,
                    sprite->plot.y / viewport.height - sprite->anchor.y, 0.0f
                ));

            auto anchor = glm::translate(glm::vec3(sprite->anchor.x, sprite->anchor.y, 0.0f));

            auto scale = glm::scale(
                glm::vec3(
                    sprite->plot.width / viewport.width, sprite->plot.height / viewport.height,
                    sprite->plot.height / viewport.height // todo, hack
                ));

            sprite->mesh_instance->emplaceChildComponent<WorldTransform>(offset * scale * anchor);
        }

        break; // only use the first camera...? #todo
    }

}

void Engine::BillboardSystem::initialize() {

    log<medium>("Initializing billboard system");

}
