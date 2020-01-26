//
// Created by root on 9/1/20.
//

#ifndef ENGINE_PHYSICSSYSTEM_H
#define ENGINE_PHYSICSSYSTEM_H

#include "PxPhysicsAPI.h"
#include "extensions/PxDefaultAllocator.h"
#include "extensions/PxDefaultErrorCallback.h"
#include "pvd/PxPvd.h"

using namespace physx;

namespace Physics {


	class PhysicsSystem {

		



	public:
		void initialize();

	private:
		void createFoundation();
		void createPhysicsObject();
		void createPVD();

	};


}

#endif //ENGINE_PHYSICSSYSTEM_H
