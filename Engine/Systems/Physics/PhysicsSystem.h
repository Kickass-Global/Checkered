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


        Component::Vehicle *playerVehicle = nullptr;

        Component::ComponentId onVehicleCreatedHandler;
        Component::ComponentId onActorCreatedHandler;
        Component::ComponentId onKeyPressHandler;
        Component::ComponentId onKeyDownHandler;
        Component::ComponentId onKeyUpHandler;

        void link(Component::ComponentId sceneComponent, physx::PxRigidDynamic *actor);
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

        

        void onKeyDown(const Component::EventArgs<int> &args);

        void onKeyUp(const Component::EventArgs<int> &args);

        void onKeyPress(const Component::EventArgs<int> &args);

        PxVehicleDrive4W *createDrivableVehicle(const PxTransform &worldTransform);
        Component::Passenger* createPassenger(const PxTransform& pickupTrans, const PxTransform& dropOffTrans);
        void onVehicleCreated(const Component::EventArgs<Component::ComponentId> &args);

        void onActorCreated(const EventArgs<Component::PhysicsActor *, Component::Mesh *> &args);
    };



}

namespace physx {
    std::ostream &operator<<(std::ostream &out, const PxTransform &transform);
}

#endif //ENGINE_PHYSICSSYSTEM_H
