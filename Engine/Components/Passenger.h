
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

using namespace physx;

namespace Component {

	class Passenger : public Component::ComponentBase<Component::ClassId::Passenger> {

	public:
		

		PxRigidStatic* pass_actor_pickup;
		PxRigidStatic* pass_actor_dropoff;

		PxPhysics* pass_physics;
		PxMaterial* pass_material;

		PxTransform pickupTransform;
		PxTransform dropOffTransform;

		void setPickupTransform(PxTransform pickupTrans);
		void setDropoffTransform(PxTransform dropoffTrans);

	private:

		




	};


	


}
#endif
