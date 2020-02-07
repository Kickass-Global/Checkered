//
// Created by root on 9/1/20.
//

#ifndef ENGINE_PHYSICSSYSTEM_H
#define ENGINE_PHYSICSSYSTEM_H

#include "PxPhysicsAPI.h"
#include "extensions/PxDefaultAllocator.h"
#include "extensions/PxDefaultErrorCallback.h"
#include "pvd/PxPvd.h"
#include "cooking/PxCooking.h"
#include "snippetvehiclecommon/SnippetVehicleFilterShader.h"
#include "snippetvehiclecommon/SnippetVehicleCreate.h"
#include "vehicle/PxVehicleDrive4W.h"
#include "snippetvehiclecommon/SnippetVehicleSceneQuery.h"
#include "vehicle/PxVehicleUpdate.h"
#include "../Engine.h"
#include "vehicle/PxVehicleUtil.h"



using namespace physx;

namespace Physics {


	class PhysicsSystem {


		
		



	public:
		void initialize();

	private:
		void createFoundation();
		void createPhysicsObject();
		void createPVD();
		void createCooking();
		void createScene();
		void createGround();
		void initVehicleSupport();
		void createDrivableVehicle();
		void stepPhysics(Engine::frametime);
	};


}

#endif //ENGINE_PHYSICSSYSTEM_H
