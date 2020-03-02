
#ifndef PASSENGER_H
#define PASSENGER_H

#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
#include <PxShape.h>
#include <PxRigidActor.h>
#include <PxRigidBody.h>
#include "ComponentId.h"
#include "Component.h"
#include "Engine.h"
#include "Physics/PhysicsSystem.h"

using namespace physx;

namespace Component {

	class Passenger : public Component::ComponentBase<Component::ClassId::Passenger> {

	public:
		

		PxRigidStatic* pass_actor;
		PxShape* pass_shape;
		PxPhysics* pass_physics;
		PxMaterial* pass_material;

		PxTransform pickupTransform;
		PxTransform dropOffTransform;


	private:

		




	};


}
#endif
