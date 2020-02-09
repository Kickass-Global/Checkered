//
// Created by Jackson Cougar Wiebe on 2/8/2020.
//

#include <algorithm>
#include <set>

#include "scenecomponentsystem.hpp"
#include "../../SystemCalls.h"
#include "../../Components/Dirty.h"
#include "../../Components/SceneComponent.h"
#include "../../Components/Component.h"
#include "../../Components/physicshandler.hpp"

void Component::SceneComponentSystem::update(Engine::deltaTime) {

    // Go through every SceneComponent in the scene and update its transformation matrix, then update each component
    // attached to the SceneComponent with the new world transform.

    auto sceneComponents = Component::Index::entitiesOf(Component::ClassId::SceneComponent);
    for (const auto &sceneComponent : sceneComponents) {

        auto is_dirty = sceneComponent.hasChildComponent(Component::Dirty::id());
        auto physicsUpdates = sceneComponent.childComponentsOfClass(Component::ClassId::PhysicsPacket);
        auto has_physics_update = !physicsUpdates.empty();

        auto meta = sceneComponent.data<Component::SceneComponent>();

        if (has_physics_update) {

            //Engine::log("Performing physics update #", sceneComponent);
            // todo something here.
            auto translation = physicsUpdates.begin()->data<Component::PhysicsPacket>()->position;
            meta->m_localTransform = glm::translate(translation);
        }

        for (const auto &update : physicsUpdates) sceneComponent.destroyComponent(update);

        if (is_dirty) {

            Engine::log("Updating scene component worldTransform #", sceneComponent);

            auto transform = meta->getWorldTransform();

            auto meshes = sceneComponent.childComponentsOfClass(Component::ClassId::Mesh);

            for (const auto &mesh : meshes) {

                Engine::log("Updating attached component world transform #", mesh);

                auto worldTransform = Engine::createComponent<Component::WorldTransform>();
                worldTransform->world_matrix = transform;
                mesh.attachExistingComponent(worldTransform->id());
                mesh.attachExistingComponent(Component::Dirty::id());

            }

            auto models = sceneComponent.childComponentsOfClass(Component::ClassId::Model);

            for (const auto &model : models) {

                Engine::log("Updating attached component world transform #", model);

                auto worldTransform = Engine::createComponent<Component::WorldTransform>();
                worldTransform->world_matrix = transform;
                model.destroyComponentsOfType(Component::ClassId::Transform);
                model.attachExistingComponent(worldTransform->id());
                model.attachExistingComponent(Component::Dirty::id());

            }
        }
        sceneComponent.destroyComponent(Component::Dirty::id());
    }
}
void Component::SceneComponentSystem::initialize() {
    Engine::log<module>("Initializing system");
}
