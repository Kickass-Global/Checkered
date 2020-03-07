
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
		EventDelegate<int> onPassengerPickedUpDelegate{""};
		EventDelegate<int> onPassengerDroppedOffDelegate{""};

		PxPhysics* pass_physics;
		PxMaterial* pass_material;

		PxTransform pickupTransform;
		PxTransform dropOffTransform;

		void setPickupTransform(PxTransform pickupTrans);
		void setDropoffTransform(PxTransform dropoffTrans);

		Passenger(glm::vec3 pickup_location, glm::vec3 dropoff_location, std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) {

			pickup_actor = Engine::createComponent<Waypoint>(glm::vec3{ 0.f, 0.f, 2.0f }, mesh, material);
			dropoff_actor = Engine::createComponent<Waypoint>(glm::vec3{ 13.0f, 0.f, 2.0f }, mesh, material);
			pickup_actor->onEntered += std::bind(&Passenger::onPassengerPickedUp, this, std::placeholders::_1);
			dropoff_actor->onEntered += std::bind(&Passenger::onPassengerDroppedOff, this, std::placeholders::_1);
			dropoff_actor->mesh->enabled = false;
		}

	private:

		void onPassengerPickedUp(PhysicsActor*) {
			using namespace Engine;
			log<high>("Passenger picked up"); 
			dropoff_actor->mesh->enabled = true;
			pickup_actor->mesh->enabled = false;
			onPassengerPickedUpDelegate(0);
		}


		void onPassengerDroppedOff(PhysicsActor*) {
			using namespace Engine;
			log<high>("Passenger dropped off");
			pickup_actor->mesh->enabled = false;
			dropoff_actor->mesh->enabled = false;
			onPassengerDroppedOffDelegate(0);
		}

	};





}
#endif
