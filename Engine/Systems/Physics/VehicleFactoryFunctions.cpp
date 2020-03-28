//
// Created by Jackson Cougar Wiebe on 3/8/2020.
//

#include "FilterShader.h"
#include "VehicleFactoryFunctions.hpp"

using namespace physx;

PxConvexMesh *Engine::createConvexMesh(
    const PxVec3 *verts, const PxU32 numVerts, PxPhysics &physics, PxCooking &cooking
) {
    // Create descriptor for convex mesh
    PxConvexMeshDesc convexDesc;
    convexDesc.points.count = numVerts;
    convexDesc.points.stride = sizeof(PxVec3);
    convexDesc.points.data = verts;
    convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

    PxConvexMesh *convexMesh = NULL;
    PxDefaultMemoryOutputStream buf;
    if (cooking.cookConvexMesh(convexDesc, buf)) {
        PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
        convexMesh = physics.createConvexMesh(id);
    }

    return convexMesh;
}


PxConvexMesh *Engine::createChassisMesh(const PxVec3 dims, PxPhysics &physics, PxCooking &cooking) {

    const PxF32 x = dims.x * 0.5f;
    const PxF32 y = dims.y * 0.5f;
    const PxF32 z = dims.z * 0.5f;
    PxVec3 verts[8] = {PxVec3(x, y, -z), PxVec3(x, y, z), PxVec3(x, -y, z), PxVec3(x, -y, -z), PxVec3(-x, y, -z),
                       PxVec3(-x, y, z), PxVec3(-x, -y, z), PxVec3(-x, -y, -z)};

    return createConvexMesh(verts, 8, physics, cooking);
}

void Engine::computeWheelCenterActorOffsets4W(
    const PxF32 wheelFrontZ, const PxF32 wheelRearZ, const PxVec3 &chassisDims, const PxF32 wheelWidth,
    const PxF32 wheelRadius, const PxU32 numWheels, PxVec3 *wheelCentreOffsets
) {
    //chassisDims.z is the distance from the rear of the chassis to the front of the chassis.
    //The front has z = 0.5*chassisDims.z and the rear has z = -0.5*chassisDims.z.
    //Compute a position for the front wheel and the rear wheel along the z-axis.
    //Compute the separation between each wheel along the z-axis.
    const PxF32 numLeftWheels = numWheels / 2.0f;
    const PxF32 deltaZ = (wheelFrontZ - wheelRearZ) / (numLeftWheels - 1.0f);
    //Set the outside of the left and right wheels to be flush with the chassis.
    //Set the top of the wheel to be just touching the underside of the chassis.
    //Begin by setting the rear-left/rear-right/front-left,front-right wheels.
    wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_LEFT] = PxVec3((-chassisDims.x + wheelWidth) * 0.5f,
                                                                        -(chassisDims.y / 2 + wheelRadius),
                                                                        wheelRearZ + 0 * deltaZ * 0.5f
    );
    wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_RIGHT] = PxVec3((+chassisDims.x - wheelWidth) * 0.5f,
                                                                         -(chassisDims.y / 2 + wheelRadius),
                                                                         wheelRearZ + 0 * deltaZ * 0.5f
    );
    wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_LEFT] = PxVec3((-chassisDims.x + wheelWidth) * 0.5f,
                                                                         -(chassisDims.y / 2 + wheelRadius),
                                                                         wheelRearZ + (numLeftWheels - 1) * deltaZ
    );
    wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT] = PxVec3((+chassisDims.x - wheelWidth) * 0.5f,
                                                                          -(chassisDims.y / 2 + wheelRadius),
                                                                          wheelRearZ + (numLeftWheels - 1) * deltaZ
    );
    //Set the remaining wheels.
    for (PxU32 i = 2, wheelCount = 4; i < numWheels - 2; i += 2, wheelCount += 2) {
        wheelCentreOffsets[wheelCount + 0] = PxVec3((-chassisDims.x + wheelWidth) * 0.5f,
                                                    -(chassisDims.y / 2 + wheelRadius), wheelRearZ + i * deltaZ * 0.5f
        );
        wheelCentreOffsets[wheelCount + 1] = PxVec3((+chassisDims.x - wheelWidth) * 0.5f,
                                                    -(chassisDims.y / 2 + wheelRadius), wheelRearZ + i * deltaZ * 0.5f
        );
    }
}

void Engine::setupWheelsSimulationData(
    const PxF32 wheelMass, const PxF32 wheelMOI, const PxF32 wheelRadius, const PxF32 wheelWidth, const PxU32 numWheels,
    const PxVec3 *wheelCenterActorOffsets, const PxVec3 &chassisCMOffset, const PxF32 chassisMass,
    PxVehicleWheelsSimData *wheelsSimData
) {
    //Set up the wheels.
    PxVehicleWheelData wheels[PX_MAX_NB_WHEELS];
    {
        //Set up the wheel data structures with mass, moi, radius, width.
        for (PxU32 i = 0; i < numWheels; i++) {
            wheels[i].mMass = wheelMass;
            wheels[i].mMOI = wheelMOI;
            wheels[i].mRadius = wheelRadius;
            wheels[i].mWidth = wheelWidth;
        }

        //Enable the handbrake for the rear wheels only.
        wheels[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque = 4000.0f;
        wheels[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque = 4000.0f;
        //Enable steering for the front wheels only.
        wheels[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = PxPi * 0.3333f;
        wheels[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = PxPi * 0.3333f;
    }

    //Set up the tires.
    PxVehicleTireData tires[PX_MAX_NB_WHEELS];
    {
        //Set up the tires.
        for (PxU32 i = 0; i < numWheels; i++) {
            tires[i].mType = Engine::TIRE_TYPE_NORMAL;
        }
    }

    //Set up the suspensions
    PxVehicleSuspensionData suspensions[PX_MAX_NB_WHEELS];
    {
        //Compute the mass supported by each suspension spring.
        PxF32 suspSprungMasses[PX_MAX_NB_WHEELS];
        PxVehicleComputeSprungMasses(
            numWheels, wheelCenterActorOffsets, chassisCMOffset, chassisMass, 1, suspSprungMasses
        );

        //Set the suspension data.
        for (PxU32 i = 0; i < numWheels; i++) {
            suspensions[i].mMaxCompression = 0.3f;
            suspensions[i].mMaxDroop = 0.1f;
            suspensions[i].mSpringStrength = 35000.0f;
            suspensions[i].mSpringDamperRate = 4500.0f;
            suspensions[i].mSprungMass = suspSprungMasses[i];
        }

        //Set the camber angles.
        const PxF32 camberAngleAtRest = 0.0;
        const PxF32 camberAngleAtMaxDroop = 0.01f;
        const PxF32 camberAngleAtMaxCompression = -0.01f;
        for (PxU32 i = 0; i < numWheels; i += 2) {
            suspensions[i + 0].mCamberAtRest = camberAngleAtRest;
            suspensions[i + 1].mCamberAtRest = -camberAngleAtRest;
            suspensions[i + 0].mCamberAtMaxDroop = camberAngleAtMaxDroop;
            suspensions[i + 1].mCamberAtMaxDroop = -camberAngleAtMaxDroop;
            suspensions[i + 0].mCamberAtMaxCompression = camberAngleAtMaxCompression;
            suspensions[i + 1].mCamberAtMaxCompression = -camberAngleAtMaxCompression;
        }
    }

    //Set up the wheel geometry.
    PxVec3 suspTravelDirections[PX_MAX_NB_WHEELS];
    PxVec3 wheelCentreCMOffsets[PX_MAX_NB_WHEELS];
    PxVec3 suspForceAppCMOffsets[PX_MAX_NB_WHEELS];
    PxVec3 tireForceAppCMOffsets[PX_MAX_NB_WHEELS];
    {
        //Set the geometry data.
        for (PxU32 i = 0; i < numWheels; i++) {
            //Vertical suspension travel.
            suspTravelDirections[i] = PxVec3(0, -1, 0);

            //Wheel center offset is offset from rigid body center of mass.
            wheelCentreCMOffsets[i] = wheelCenterActorOffsets[i] - chassisCMOffset;

            //Suspension force application point 0.3 metres below
            //rigid body center of mass.
            suspForceAppCMOffsets[i] = PxVec3(
                wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z
            );

            //Tire force application point 0.3 metres below
            //rigid body center of mass.
            tireForceAppCMOffsets[i] = PxVec3(
                wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z
            );
        }
    }

    //Set up the filter data of the raycast that will be issued by each suspension.
    PxFilterData qryFilterData;
    qryFilterData.word3 = Engine::QueryFilterMask::eNotDrivable;

    //Set the wheel, tire and suspension data.
    //Set the geometry data.
    //Set the query filter data
    for (PxU32 i = 0; i < numWheels; i++) {
        wheelsSimData->setWheelData(i, wheels[i]);
        wheelsSimData->setTireData(i, tires[i]);
        wheelsSimData->setSuspensionData(i, suspensions[i]);
        wheelsSimData->setSuspTravelDirection(
            i, suspTravelDirections[i]
        );
        wheelsSimData->setWheelCentreOffset(i, wheelCentreCMOffsets[i]);
        wheelsSimData->setSuspForceAppPointOffset(
            i, suspForceAppCMOffsets[i]
        );
        wheelsSimData->setTireForceAppPointOffset(
            i, tireForceAppCMOffsets[i]
        );
        wheelsSimData->setSceneQueryFilterData(i, qryFilterData);
        wheelsSimData->setWheelShapeMapping(i, PxI32(i));
    }

    //Add a front and rear anti-roll bar
    PxVehicleAntiRollBarData barFront;
    barFront.mWheel0 = PxVehicleDrive4WWheelOrder::eFRONT_LEFT;
    barFront.mWheel1 = PxVehicleDrive4WWheelOrder::eFRONT_RIGHT;
    barFront.mStiffness = 10000.0f;
    wheelsSimData->addAntiRollBarData(barFront);
    PxVehicleAntiRollBarData barRear;
    barRear.mWheel0 = PxVehicleDrive4WWheelOrder::eREAR_LEFT;
    barRear.mWheel1 = PxVehicleDrive4WWheelOrder::eREAR_RIGHT;
    barRear.mStiffness = 10000.0f;
    wheelsSimData->addAntiRollBarData(barRear);
}

PxRigidDynamic *createVehicleActor(
    const PxVehicleChassisData &chassisData, PxMaterial **wheelMaterials, PxConvexMesh **wheelConvexMeshes,
    const PxU32 numWheels, const PxFilterData &wheelSimFilterData, PxMaterial **chassisMaterials,
    PxConvexMesh **chassisConvexMeshes, const PxU32 numChassisMeshes, const PxFilterData &chassisSimFilterData,
    PxPhysics &physics
) {

    //We need a rigid body actor for the vehicle.
    //Don't forget to add the actor to the scene after setting up the associated vehicle.
    PxRigidDynamic *vehActor = physics.createRigidDynamic(
        PxTransform(PxIdentity));

    //Wheel and chassis query filter data.
    //Optional: cars don't drive on other cars.
    PxFilterData wheelQryFilterData;
    wheelQryFilterData.word3 = Engine::QueryFilterMask::eNotDrivable;
    PxFilterData chassisQryFilterData;
    wheelQryFilterData.word3 = Engine::QueryFilterMask::eNotDrivable;

    //Add all the wheel shapes to the actor.
    for (PxU32 i = 0; i < numWheels; i++) {
        PxConvexMeshGeometry geom(wheelConvexMeshes[i]);
        PxShape *wheelShape = PxRigidActorExt::createExclusiveShape(
            *vehActor, geom, *wheelMaterials[i]
        );
        wheelShape->setQueryFilterData(wheelQryFilterData);
        wheelShape->setSimulationFilterData(wheelSimFilterData);
        wheelShape->setLocalPose(PxTransform(PxIdentity));
    }

    //Add the chassis shapes to the actor.
    for (PxU32 i = 0; i < numChassisMeshes; i++) {
        PxShape *chassisShape = PxRigidActorExt::createExclusiveShape(
            *vehActor, PxConvexMeshGeometry(chassisConvexMeshes[i]), *chassisMaterials[i]
        );
        chassisShape->setQueryFilterData(chassisQryFilterData);
        chassisShape->setSimulationFilterData(chassisSimFilterData);
        chassisShape->setLocalPose(PxTransform(PxIdentity));
    }

    vehActor->setMass(chassisData.mMass);
    vehActor->setMassSpaceInertiaTensor(chassisData.mMOI);
    vehActor->setCMassLocalPose(
        PxTransform(chassisData.mCMOffset, PxQuat(PxIdentity)));

    return vehActor;
}

PxConvexMesh *Engine::createWheelMesh(
    const PxF32 width, const PxF32 radius, PxPhysics &physics, PxCooking &cooking
) {

    PxVec3 points[2 * 16];
    for (PxU32 i = 0; i < 16; i++) {
        const PxF32 cosTheta = PxCos(i * PxPi * 2.0f / 16.0f);
        const PxF32 sinTheta = PxSin(i * PxPi * 2.0f / 16.0f);
        const PxF32 y = radius * cosTheta;
        const PxF32 z = radius * sinTheta;
        points[2 * i + 0] = PxVec3(-width / 2.0f, y, z);
        points[2 * i + 1] = PxVec3(+width / 2.0f, y, z);
    }

    return createConvexMesh(points, 32, physics, cooking);
}
