//
// Created by Jackson Cougar Wiebe on 2/8/2020.
//

#include <PhysicsActor.h>

#include "scenecomponentsystem.hpp"
#include "tags.h"
#include "../../Components/SceneComponent.h"
#include "physicspacket.hpp"
#include "Engine.h"
#include "WorldTransform.h"

namespace {
    const char module[] = "ComponentSystem";
}

void process_scene_component(SceneComponent* node) {
	auto T = node->getWorldTransform();
	// foreach thing attached to this scene component, attach this transform to it...
	for (auto child : node->getChildren().getComponentsOfType<PaintedMesh>())
	{
		child->eraseChildComponentsOfType<WorldTransform>();
		child->emplaceChildComponent<WorldTransform>(T);
	}

	for (auto child : node->getChildren().getComponentsOfType<SceneComponent>())
	{
		process_scene_component(child);
	}
}

void Component::SceneComponentSystem::update(Engine::deltaTime) {

    // Go through every SceneComponent in the scene and update its transformation matrix, then update each component
    // attached to the SceneComponent with the new world transform.

	auto actors = Engine::getStore().getRoot().getComponentsOfType<PhysicsActor>();
	for (auto actor : actors)
	{
		// if actor is not initialized
		if (!actor->actor)
			onActorCreated(actor);
		actor->update();
	}



    auto sceneComponents = Engine::getStore().getRoot().getComponentsOfType<SceneComponent>();

    for (const auto &sceneComponent : sceneComponents) {
/*
        auto is_dirty = sceneComponent->
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
        }*/
    }
}

void Component::SceneComponentSystem::initialize() {
    Engine::log<module>("Initializing system");
}
