//
// Created by root on 9/1/20.
//
#pragma once

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
#include "Engine.h"
#include "vehicle/PxVehicleUtil.h"
#include "../systeminterface.hpp"
#include "EventDelegate.h"
#include "PhysicsActor.h"
#include <Vehicle.h>


using namespace physx;


namespace Physics {
	const char module[] = "Physics";

	class PhysicsSystem : public Engine::SystemInterface {

	public:
		void initialize() override;

		void update(Engine::deltaTime /*elapsed*/) override;


		std::shared_ptr<Component::Vehicle> playerVehicle;

		std::shared_ptr<EventHandler<Vehicle*>> onVehicleCreatedHandler;
		std::shared_ptr<EventHandler<PhysicsActor*>> onActorCreatedHandler;
		std::shared_ptr<EventHandler<int>> onKeyPressHandler;
		std::shared_ptr<EventHandler<int>> onKeyDownHandler;
		std::shared_ptr<EventHandler<int>> onKeyUpHandler;

		void link(Vehicle* sceneComponent, physx::PxRigidDynamic *actor);

	private:
		void createFoundation();

		void createPhysicsObject();

		void createPVD();

		void createCooking();

		void createScene();

		void createGround();

		void initVehicleSupport();

		void createDrivablePlayerVehicle();

		void stepPhysics(Engine::deltaTime);

		void onKeyDown(const EventArgs<int> &args);

		void onKeyUp(const EventArgs<int> &args);

		void onKeyPress(const EventArgs<int> &args);

		PxVehicleDrive4W *createDrivableVehicle(const PxTransform &worldTransform);

		void onVehicleCreated(const EventArgs<Vehicle*> &args);

		void onActorCreated(const EventArgs<PhysicsActor *> &args);
	};

}

namespace physx {
	std::ostream &operator<<(std::ostream &out, const PxTransform &transform);
}

#endif //ENGINE_PHYSICSSYSTEM_H
