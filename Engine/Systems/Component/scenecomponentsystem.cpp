//
// Created by Jackson Cougar Wiebe on 2/8/2020.
//

#include "scenecomponentsystem.hpp"
#include "../../SystemCalls.h"
#include "../../Components/Dirty.h"
#include "../../Components/SceneComponent.h"
#include "../../Components/Component.h"

void Component::SceneComponentSystem::update(Engine::deltaTime) {

    // Go through every SceneComponent in the scene and update its transformation matrix, then update each component
    // attached to the SceneComponent with the new world transform.

    auto sceneComponents = Component::Index::entitiesOf(Component::ClassId::SceneComponent);
    for (const auto &sceneComponent : sceneComponents) {

        auto is_dirty = sceneComponent.hasChildComponent(Component::Dirty::id());

        if (is_dirty) {

            Engine::log("Updating scene component worldTransform ", sceneComponent);

            auto meta = sceneComponent.data<Component::SceneComponent>();
            auto transform = meta->getWorldTransform();

            auto meshes = sceneComponent.childComponentsOfClass(Component::ClassId::Mesh);
            for (const auto &mesh : meshes) {

                Engine::log("Updating attached component world transform", mesh);

                auto worldTransform = Engine::createComponent<Component::WorldTransform>();
                worldTransform->world_matrix = transform;
                mesh.attachExistingComponent(worldTransform->id());

            }
        }
    }
}
void Component::SceneComponentSystem::initialize() {
    Engine::log<module>("Initializing system");
}
