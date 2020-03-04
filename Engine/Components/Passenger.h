

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

	class Passenger : public ComponentBase {

	public:
		
		void initPassenger();


	private:

		float PASS_STATIC_FRICTION = 100.0F;
		float PASS_DYNAMIC_FRICTION = 100.0f;
		float PASS_RESTITUTION = 100.0f;


		PxShape* pass_shape;
		PxRigidActor* pass_actor;
		PxPhysics* pass_physics;
		PxMaterial* pass_material;



	};


}

