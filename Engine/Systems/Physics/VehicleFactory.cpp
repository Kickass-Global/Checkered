//
// Created by Jackson Cougar Wiebe on 3/8/2020.
//

#include "VehicleFactory.hpp"
#include "VehicleFactoryFunctions.hpp"

using namespace physx;

Engine::VehicleFactory::VehicleFactory(PxPhysics *pxPhysics, PxCooking *pxCooking) : pxPhysics(
    pxPhysics
), pxCooking(pxCooking) {}

PxVehicleDrive4W *Engine::VehicleFactory::createVehicle(const struct VehicleDescription &vehicle4WDesc) {

    const PxVec3 chassisDims = vehicle4WDesc.chassisDims;
    const PxF32 wheelWidth = vehicle4WDesc.wheelWidth;
    const PxF32 wheelRadius = vehicle4WDesc.wheelRadius;
    const PxU32 numWheels = vehicle4WDesc.numWheels;

    const PxFilterData &chassisSimFilterData = vehicle4WDesc.chassisSimFilterData;
    const PxFilterData &wheelSimFilterData = vehicle4WDesc.wheelSimFilterData;

    //Construct a physx actor with shapes for the chassis and wheels.
    //Set the rigid body mass, moment of inertia, and center of mass offset.
    PxRigidDynamic *veh4WActor = NULL;
    {
        //Construct a convex mesh for a cylindrical wheel.
        PxConvexMesh *wheelMesh = Engine::createWheelMesh(
            wheelWidth, wheelRadius, *pxPhysics, *pxCooking
        );
        //Assume all wheels are identical for simplicity.
        PxConvexMesh *wheelConvexMeshes[PX_MAX_NB_WHEELS];
        PxMaterial *wheelMaterials[PX_MAX_NB_WHEELS];

        //Set the meshes and materials for the driven wheels.
        for (PxU32 i = PxVehicleDrive4WWheelOrder::eFRONT_LEFT; i <= PxVehicleDrive4WWheelOrder::eREAR_RIGHT; i++) {
            wheelConvexMeshes[i] = wheelMesh;
            wheelMaterials[i] = vehicle4WDesc.wheelMaterial;
        }
        //Set the meshes and materials for the non-driven wheels
        for (PxU32 i = PxVehicleDrive4WWheelOrder::eREAR_RIGHT + 1; i < numWheels; i++) {
            wheelConvexMeshes[i] = wheelMesh;
            wheelMaterials[i] = vehicle4WDesc.wheelMaterial;
        }

        //Chassis just has a single convex shape for simplicity.
        PxConvexMesh *chassisConvexMesh = Engine::createChassisMesh(
            chassisDims, *pxPhysics, *pxCooking
        );
        PxConvexMesh *chassisConvexMeshes[1] = {chassisConvexMesh};
        PxMaterial *chassisMaterials[1] = {vehicle4WDesc.chassisMaterial};

        //Rigid body data.
        PxVehicleChassisData rigidBodyData;
        rigidBodyData.mMOI = vehicle4WDesc.chassisMOI;
        rigidBodyData.mMass = vehicle4WDesc.chassisMass;
        rigidBodyData.mCMOffset = vehicle4WDesc.chassisCMOffset;

        veh4WActor = snippetvehicle::createVehicleActor(
            rigidBodyData, wheelMaterials, wheelConvexMeshes, numWheels, wheelSimFilterData, chassisMaterials,
            chassisConvexMeshes, 1, chassisSimFilterData, *pxPhysics, PxTransform()
        );
    }


    //Set up the sim data for the wheels.
    PxVehicleWheelsSimData *wheelsSimData = PxVehicleWheelsSimData::allocate(
        numWheels
    );
    {
        //Compute the wheel center offsets from the origin.
        PxVec3 wheelCenterActorOffsets[PX_MAX_NB_WHEELS];
        const PxF32 frontZ = chassisDims.z * 0.3f;
        const PxF32 rearZ = -chassisDims.z * 0.3f;
        Engine::computeWheelCenterActorOffsets4W(
            frontZ, rearZ, chassisDims, wheelWidth, wheelRadius, numWheels, wheelCenterActorOffsets
        );

        //Set up the simulation data for all wheels.
        setupWheelsSimulationData(
            vehicle4WDesc.wheelMass, vehicle4WDesc.wheelMOI, wheelRadius, wheelWidth, numWheels,
            wheelCenterActorOffsets, vehicle4WDesc.chassisCMOffset, vehicle4WDesc.chassisMass, wheelsSimData
        );
    }

    //Set up the sim data for the vehicle drive model.
    PxVehicleDriveSimData4W driveSimData;
    {
        //Diff
        PxVehicleDifferential4WData diff;
        diff.mType = PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
        driveSimData.setDiffData(diff);

        //Engine
        PxVehicleEngineData engine;
        engine.mPeakTorque = 500.0f;
        engine.mMaxOmega = 600.0f;//approx 6000 rpm
        driveSimData.setEngineData(engine);

        //Gears
        PxVehicleGearsData gears;
        gears.mSwitchTime = 0.5f;
        driveSimData.setGearsData(gears);

        //Clutch
        PxVehicleClutchData clutch;
        clutch.mStrength = 10.0f;
        driveSimData.setClutchData(clutch);

        //Ackermann steer accuracy
        PxVehicleAckermannGeometryData ackermann;
        ackermann.mAccuracy = 1.0f;
        ackermann.mAxleSeparation = wheelsSimData->getWheelCentreOffset(
            PxVehicleDrive4WWheelOrder::eFRONT_LEFT
        ).z - wheelsSimData->getWheelCentreOffset(
            PxVehicleDrive4WWheelOrder::eREAR_LEFT
        ).z;
        ackermann.mFrontWidth = wheelsSimData->getWheelCentreOffset(
            PxVehicleDrive4WWheelOrder::eFRONT_RIGHT
        ).x - wheelsSimData->getWheelCentreOffset(
            PxVehicleDrive4WWheelOrder::eFRONT_LEFT
        ).x;
        ackermann.mRearWidth = wheelsSimData->getWheelCentreOffset(
            PxVehicleDrive4WWheelOrder::eREAR_RIGHT
        ).x - wheelsSimData->getWheelCentreOffset(
            PxVehicleDrive4WWheelOrder::eREAR_LEFT
        ).x;
        driveSimData.setAckermannGeometryData(ackermann);
    }

    //Create a vehicle from the wheels and drive sim data.
    PxVehicleDrive4W *vehDrive4W = PxVehicleDrive4W::allocate(numWheels);
    vehDrive4W->setup(
        pxPhysics, veh4WActor, *wheelsSimData, driveSimData, numWheels - 4
    );

    //Free the sim data because we don't need that any more.
    wheelsSimData->free();

    return vehDrive4W;
}


