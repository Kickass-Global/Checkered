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
	for (auto &sceneComponent : sceneComponents) {
		process_scene_component(sceneComponent);
	}
}


void Component::SceneComponentSystem::initialize() {
	Engine::log<module>("Initializing system");
}
