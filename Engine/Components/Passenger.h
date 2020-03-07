
#ifndef PASSENGER_H
#define PASSENGER_H

#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
#include <PxShape.h>
#include <PxRigidActor.h>
#include <PxRigidBody.h>
#include "ComponentId.h"
#include "Component.h"
#include "scenery.hpp"
#include "Engine.h"

using namespace physx;

namespace Component {

	class Passenger : public ComponentBase {

	public:
		
		std::shared_ptr<Waypoint> pickup_actor;
		std::shared_ptr<Waypoint> dropoff_actor;

		PxPhysics* pass_physics;
		PxMaterial* pass_material;

		PxTransform pickupTransform;
		PxTransform dropOffTransform;

		void setPickupTransform(PxTransform pickupTrans);
		void setDropoffTransform(PxTransform dropoffTrans);

		Passenger() {
			//pickup_actor->onEntered += std::bind(&Passenger::onPassengerPickedUp, this, std::placeholders::_1);
		}

	private:

		void onPassengerPickedUp(PhysicsActor*) {
			using namespace Engine;
			log<high>("Passenger picked up");
		}




	};


	


}
#endif
