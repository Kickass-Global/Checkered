//
// Created by Jackson Cougar Wiebe on 2/8/2020.
//

#include <algorithm>
#include <set>

#include "scenecomponentsystem.hpp"
#include "tags.h"
#include "../../Components/SceneComponent.h"
#include "../../Components/Component.h"
#include "physicspacket.hpp"
#include "Engine.h"
#include "WorldTransform.h"

namespace {
    const char module[] = "ComponentSystem";
}

void Component::SceneComponentSystem::update(Engine::deltaTime) {

    // Go through every SceneComponent in the scene and update its transformation matrix, then update each component
    // attached to the SceneComponent with the new world transform.

    auto sceneComponents = Component::Index::entitiesOf<SceneComponent>();

    for (const auto &sceneComponent : sceneComponents) {

        auto is_dirty = sceneComponent.hasTag<Component::Dirty>(true);
        auto physicsUpdates = sceneComponent.childComponentsOfClass(Component::ClassId::PhysicsPacket);
        auto has_physics_update = !physicsUpdates.empty();

        auto meta = sceneComponent.data<Component::SceneComponent>();

        if (has_physics_update) {

            auto physx_data = physicsUpdates.begin()->data<Component::PhysicsPacket>();
            auto translation = physx_data->position;
            meta->m_localTransform = glm::translate(translation) * glm::mat4_cast(physx_data->rotation);
        }

        for (const auto &update : physicsUpdates) sceneComponent.destroyComponent(update);

        if (is_dirty) {

            Engine::log<module>("Updating scene component worldTransform #", sceneComponent);

            auto transform = meta->getWorldTransform();

            auto meshes = sceneComponent.childComponentsOfClass(Component::ClassId::Mesh);

            for (const auto &mesh : meshes) {

                Engine::log<module>("Updating attached component world transform #", mesh);

                auto worldTransform = Engine::createComponentWithTTL<Component::WorldTransform>(3);
                worldTransform->world_matrix = transform;
                mesh.attachExistingComponent(worldTransform->id());

            }

            auto models = sceneComponent.childComponentsOfClass(Component::ClassId::Model);

            for (const auto &model : models) {

                Engine::log("Updating attached component world transform #", model);

                auto worldTransform = Engine::createComponentWithTTL<Component::WorldTransform>(3);
                worldTransform->world_matrix = transform;
                model.destroyComponentsOfType(Component::ClassId::Transform);
                model.attachExistingComponent(worldTransform->id());

            }
        }
    }
}
void Component::SceneComponentSystem::initialize() {
    Engine::log<module>("Initializing system");
}
