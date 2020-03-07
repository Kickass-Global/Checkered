//
// Created by Jackson Cougar Wiebe on 3/1/2020.
//

#ifndef ENGINE_PHYSICSACTOR_H
#define ENGINE_PHYSICSACTOR_H

#include <Component.h>
#include <PxPhysicsAPI.h>
#include "Mesh.h"
#include <memory>
#include <SceneComponent.h>

namespace Component {
	class PhysicsActor : public ComponentBase {

	public:

		EventDelegate<PhysicsActor*, PhysicsActor*> onBeginOverlap{ "onBeginOverlap" };
		EventDelegate<PhysicsActor*, PhysicsActor*> onEndOverlap{ "onEndOverlap" };

		std::shared_ptr<SceneComponent> node = Engine::createComponent<SceneComponent>(nullptr);
		// this will be used to generate the collision bounds, may be different than the visual mesh.
		std::shared_ptr<Mesh> mesh;
		physx::PxRigidActor* actor;
		glm::vec3 position = { 0,0,0 };
		glm::quat rotation = glm::quat(1, 0, 0, 0);
		
		enum Type {
			Ground,
			DynamicObject,
			StaticObject,
			TriggerVolume
		} type = StaticObject;

		glm::mat4 getWorldTransform();

		void update() {
			node->m_localTransform = getWorldTransform();
		}

		explicit PhysicsActor(std::shared_ptr<Component::Mesh>& mesh);
	};
}


#endif //ENGINE_PHYSICSACTOR_H
