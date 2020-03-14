#pragma once
#ifndef FILTER_SHADER_H
#define FILTER_SHADER_H

#include <PxPhysicsAPI.h>
#include <PxFiltering.h>
#include <PxRigidDynamic.h>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <PxSimulationEventCallback.h>
#include <Vehicle.h>
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
#include "SystemInterface.hpp"
#include "EventDelegate.h"
#include "PhysicsActor.h"
#include "SimulationCallback.h"
#include "Passenger.h"

using namespace physx;

namespace Physics {

    class FilterShader : public PxSimulationFilterCallback {

    public:

        static PxFilterFlags setupFilterShader(
            PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1,
            PxFilterData filterData1, PxPairFlags &pairFlags, const void *constantBlock, PxU32 constantBlockSize
        );

        static void setupQueryFiltering(PxRigidActor *actor, PxU32 filterGroup, PxU32 filterMask);

        static void setupFiltering(PxRigidActor *actor, PxU32 filterGroup, PxU32 filterMask);
    };

}

namespace Engine {
    enum FilterGroup : PxU32 {
        ePlayerVehicle = (1 << 0),
        eEnemyVehicle = (1 << 1),
        ePasenger = (1 << 2),
        eScenery = (1 << 3),
        eObstacle = (1 << 4),
        eGround = (1 << 5),
        eTrigger = (1 << 6),
        eWheel = (1 << 7),
    };

    template<class T>
    constexpr int operator|(T a, enum FilterGroup b) {

        using G = std::underlying_type_t<enum FilterGroup>;
        return static_cast<PxU32>(static_cast<G>(a) | static_cast<G>(b));
    }

    enum FilterMask : PxU32 {
        eVehicle = FilterGroup::ePlayerVehicle | FilterGroup::eEnemyVehicle,

        ePlayerColliders = eVehicle | eObstacle | eScenery | eGround | eTrigger,
        eEnemyColliders = eVehicle | eObstacle | eScenery | eGround,

        eGroundColliders = eObstacle | eScenery | eGround | eVehicle,
        eObstacleColliders = eVehicle | eObstacle | eScenery | eGround,
        eSceneryColliders = eVehicle | eObstacle | eScenery,

        eWheelColliders = eWheel,
        eTriggerColliders = ePlayerVehicle,

        eEverything = 0xFFFFFFFF,
        eNone = 0,
    };
    enum QueryFilterMask : PxU32 {
        eDrivable = 0xFFFF0000, eNotDrivable = 0x0000FFFF,
    };
}

#endif //FILTER_SHADER_H