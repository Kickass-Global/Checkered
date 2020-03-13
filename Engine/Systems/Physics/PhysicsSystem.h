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

    struct PhysicsSystem : public Engine::SystemInterface {

        PxPhysics *cPhysics = NULL;
        PxScene *cScene = NULL;
        PxTriggerPair *playPassTriggerPair;

        void initialize() override;

        void update(Engine::deltaTime /*elapsed*/) override;

        std::shared_ptr<EventHandler<Vehicle *>> onVehicleCreatedHandler;
        std::shared_ptr<EventHandler<PhysicsActor *>> onActorCreatedHandler;
        std::shared_ptr<EventHandler<int>> onKeyPressHandler;
        std::shared_ptr<EventHandler<int>> onKeyDownHandler;
        std::shared_ptr<EventHandler<int>> onKeyUpHandler;

        void link(Vehicle *sceneComponent, physx::PxRigidDynamic *actor);


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

        PxVehicleDrive4W *createDrivableVehicle(const PxTransform &worldTransform, bool is_player);

        void onVehicleCreated(const EventArgs<Vehicle *> &args);

        Component::Passenger *createPassenger(const PxTransform &pickupTrans, const PxTransform &dropOffTrans);

        void onActorCreated(const EventArgs<Component::PhysicsActor *> &args);

        PxTriangleMesh *createTriMesh(Mesh *mesh);

        PxConvexMesh *createConvexMesh(Mesh *mesh);

        void onPassengerCreated(Passenger *passenger);
    };



}

namespace physx {
	std::ostream &operator<<(std::ostream &out, const PxTransform &transform);
}


#endif //ENGINE_PHYSICSSYSTEM_H

