//
// Created by root on 9/1/20.
//
#pragma once

#ifndef ENGINE_PHYSICSSYSTEM_H
#define ENGINE_PHYSICSSYSTEM_H

#include <cstdlib>
#include "PxPhysicsAPI.h"
#include <PxSimulationEventCallback.h>
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
#include "FilterShader.h"
#include "SimulationCallback.h"
#include "Passenger.h"



using namespace physx;


namespace Physics {
	const char module[] = "Physics";

	class PhysicsSystem : public Engine::SystemInterface {

	public:

		PxPhysics* cPhysics = NULL;
		PxScene* cScene = NULL;
		PxTriggerPair* playPassTriggerPair;


		void initialize() override;

		void update(Engine::deltaTime /*elapsed*/) override;


		std::shared_ptr<Component::Vehicle> playerVehicle;

		std::shared_ptr<EventHandler<Vehicle*>> onVehicleCreatedHandler;
		std::shared_ptr<EventHandler<PhysicsActor*>> onActorCreatedHandler;
		std::shared_ptr<EventHandler<int>> onKeyPressHandler;
		std::shared_ptr<EventHandler<int>> onKeyDownHandler;
		std::shared_ptr<EventHandler<int>> onKeyUpHandler;

		void link(Vehicle* sceneComponent, physx::PxRigidDynamic *actor);

		void onPassengerCreated(Component::Passenger *pass);


	private:
		void createFoundation();
		void createPhysicsCallbacks();
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
		Component::Passenger* createPassenger(const PxTransform& pickupTrans, const PxTransform& dropOffTrans);
		void onActorCreated(const EventArgs<Component::PhysicsActor*>& args);

		PxTriangleMesh* createTriMesh(Mesh* mesh);
		PxConvexMesh* createConvexMesh(Mesh* mesh);
	};



}

namespace physx {
	std::ostream &operator<<(std::ostream &out, const PxTransform &transform);
}

enum FilterGroup : PxU32
{
	ePlayerVehicle = (1 << 0),
	eEnemyVehicle = (1 << 1),
	ePasenger = (1 << 2),
	eScenery = (1 << 3),
	eObstacle = (1 << 4),
	eGround = (1 << 5),
	eTrigger = (1 << 6),
	eWheel = (1 << 7),
};

template <class T>
constexpr int operator| (T a, enum FilterGroup b) {
	using G = std::underlying_type_t<enum FilterGroup>;
	return static_cast<PxU32>(static_cast<G>(a) | static_cast<G>(b));
}

enum FilterMask : PxU32 {
	eVehicle = FilterGroup::ePlayerVehicle | FilterGroup::eEnemyVehicle,

	eVehicleColliders = eVehicle | eObstacle | eScenery | eGround | eTrigger,
	eGroundColliders = eObstacle | eScenery | eGround | eVehicle,
	eObstacleColliders = eVehicle | eObstacle | eScenery | eGround,
	eSceneryColliders = eVehicle | eObstacle | eScenery,
	eWheelColliders = eWheel,
	eTriggerColliders = eVehicle,

	eEverything = 0xFFFFFFFF,
	eNone = 0,
};

enum QueryFilterMask : PxU32 {
	eDrivable = 0xFFFF0000,
};




#endif //ENGINE_PHYSICSSYSTEM_H

