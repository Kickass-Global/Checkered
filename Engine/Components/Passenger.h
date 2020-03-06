
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
#include "scenery.hpp"

using namespace physx;

namespace Component {

	class Passenger : public ComponentBase {

	public:
		
		std::shared_ptr<Scenery> pickup_actor;
		std::shared_ptr<Scenery> dropoff_actor;

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
