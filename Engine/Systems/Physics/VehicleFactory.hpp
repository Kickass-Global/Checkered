//
// Created by Jackson Cougar Wiebe on 3/8/2020.
//

#ifndef ENGINE_VEHICLEFACTORY_HPP
#define ENGINE_VEHICLEFACTORY_HPP

#include "PxPhysicsAPI.h"
#include "vehicle/PxVehicleDrive4W.h"
#include "vehicle/PxVehicleUtil.h"
#include "vehicle/PxVehicleUpdate.h"
#include <Vehicle.h>
#include "FilterShader.h"

namespace Engine {

    struct VehicleFactory {

        physx::PxPhysics *pxPhysics;
        physx::PxCooking *pxCooking;

        VehicleFactory(physx::PxPhysics *pxPhysics, physx::PxCooking *pxCooking);

        physx::PxVehicleDrive4W *createVehicle(const struct VehicleDescription &vehicle4WDesc);
    };

    /**
     * Lists all parameters required to create a new physx vehicle
     */
    struct VehicleDescription {
        VehicleDescription() = default;

        VehicleDescription(
            const physx::PxFilterData &chassisSimFilterData, const physx::PxFilterData &wheelSimFilterData,
            physx::PxMaterial *chassisMaterial, physx::PxMaterial *wheelMaterial
        ) : chassisMass(200), chassisDims(2.0f, 3.0f, 4.0f),
            chassisMOI((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
                       (chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
                       (chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f
            ), chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.05f, 0.25f), chassisMaterial(chassisMaterial),
            chassisSimFilterData(chassisSimFilterData), wheelMass(50), wheelWidth(0.35f), wheelRadius(0.35f),
            wheelMOI(0.35f), wheelMaterial(wheelMaterial), numWheels(4), wheelSimFilterData(wheelSimFilterData) {}

        physx::PxF32 chassisMass;
        physx::PxVec3 chassisDims;
        physx::PxVec3 chassisMOI;
        physx::PxVec3 chassisCMOffset;
        physx::PxMaterial *chassisMaterial;
        physx::PxFilterData chassisSimFilterData;

        physx::PxF32 wheelMass;
        physx::PxF32 wheelWidth;
        physx::PxF32 wheelRadius;
        physx::PxF32 wheelMOI;
        physx::PxMaterial *wheelMaterial;
        physx::PxU32 numWheels{};
        physx::PxFilterData wheelSimFilterData;
    };
}

#endif //ENGINE_VEHICLEFACTORY_HPP
