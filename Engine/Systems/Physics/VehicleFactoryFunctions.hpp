//
// Created by Jackson Cougar Wiebe on 3/8/2020.
//

#ifndef ENGINE_VEHICLEFACTORYFUNCTIONS_HPP
#define ENGINE_VEHICLEFACTORYFUNCTIONS_HPP

#include "PhysicsSystem.h"
#include "FilterShader.h"

namespace Engine {
    enum {
        SURFACE_TYPE_TARMAC, MAX_NUM_SURFACE_TYPES
    };

    enum {
        TIRE_TYPE_NORMAL = 0, TIRE_TYPE_WORN, MAX_NUM_TIRE_TYPES
    };

    physx::PxConvexMesh *createConvexMesh(
        const physx::PxVec3 *verts, const physx::PxU32 numVerts, physx::PxPhysics &physics, physx::PxCooking &cooking
    );

    void setupWheelsSimulationData(
        const PxF32 wheelMass, const PxF32 wheelMOI, const PxF32 wheelRadius, const PxF32 wheelWidth,
        const PxU32 numWheels, const PxVec3 *wheelCenterActorOffsets, const PxVec3 &chassisCMOffset,
        const PxF32 chassisMass, PxVehicleWheelsSimData *wheelsSimData
    );

    physx::PxConvexMesh *createChassisMesh(
        const physx::PxVec3 dims, physx::PxPhysics &physics, physx::PxCooking &cooking
    );

    PxConvexMesh *createWheelMesh(
        const PxF32 width, const PxF32 radius, PxPhysics &physics, PxCooking &cooking
    );

    void computeWheelCenterActorOffsets4W(
        const PxF32 wheelFrontZ, const PxF32 wheelRearZ, const PxVec3 &chassisDims, const PxF32 wheelWidth,
        const PxF32 wheelRadius, const PxU32 numWheels, PxVec3 *wheelCentreOffsets
    );
}
#endif //ENGINE_VEHICLEFACTORYFUNCTIONS_HPP
