//
// Created by root on 9/1/20.
//

#include <iostream>
#include <GLFW/glfw3.h>
#include <Vehicle.h>

#include "PhysicsSystem.h"
#include "PxRigidStatic.h"
#include "Events/Events.h"
#include "physicshandler.hpp"
#include "snippetvehiclecommon/SnippetVehicleTireFriction.h"
#include "Dirty.h"
#include "Engine.h"

using namespace physx;
using namespace snippetvehicle;

const float GRAVITY = -9.81;
const float STATIC_FRICTION = 0.5F;
const float DYNAMIC_FRICTION = 0.5f;
const float RESTITUTION = 0.6f;

namespace {
	const char module[] = "Physics";
}

PxVehicleDrivableSurfaceToTireFrictionPairs* cFrictionPairs = NULL;

PxF32 cSteerVsForwardSpeedData[2 * 8] =
        {
                0.0f, 0.75f,
                5.0f, 0.75f,
                30.0f, 0.125f,
                120.0f, 0.1f,
                PX_MAX_F32, PX_MAX_F32,
                PX_MAX_F32, PX_MAX_F32,
                PX_MAX_F32, PX_MAX_F32,
                PX_MAX_F32, PX_MAX_F32
        };
PxFixedSizeLookupTable<8> cSteerVsForwardSpeedTable(cSteerVsForwardSpeedData, 4);

PxVehicleKeySmoothingData cKeySmoothingData =
{
		{
				6.0f,    //rise rate eANALOG_INPUT_ACCEL
				6.0f,    //rise rate eANALOG_INPUT_BRAKE
				6.0f,    //rise rate eANALOG_INPUT_HANDBRAKE
				2.5f,    //rise rate eANALOG_INPUT_STEER_LEFT
				2.5f,    //rise rate eANALOG_INPUT_STEER_RIGHT
		},
		{
				10.0f,    //fall rate eANALOG_INPUT_ACCEL
				10.0f,    //fall rate eANALOG_INPUT_BRAKE
				10.0f,    //fall rate eANALOG_INPUT_HANDBRAKE
				5.0f,    //fall rate eANALOG_INPUT_STEER_LEFT
				5.0f    //fall rate eANALOG_INPUT_STEER_RIGHT
		}
};

PxVehiclePadSmoothingData cPadSmoothingData =
{
		{
				6.0f,    //rise rate eANALOG_INPUT_ACCEL
				6.0f,    //rise rate eANALOG_INPUT_BRAKE
				6.0f,    //rise rate eANALOG_INPUT_HANDBRAKE
				2.5f,    //rise rate eANALOG_INPUT_STEER_LEFT
				2.5f,    //rise rate eANALOG_INPUT_STEER_RIGHT
		},
		{
				10.0f,    //fall rate eANALOG_INPUT_ACCEL
				10.0f,    //fall rate eANALOG_INPUT_BRAKE
				10.0f,    //fall rate eANALOG_INPUT_HANDBRAKE
				5.0f,    //fall rate eANALOG_INPUT_STEER_LEFT
				5.0f    //fall rate eANALOG_INPUT_STEER_RIGHT
		}
};

PxVehicleDrive4WRawInputData cVehicleInputData;

PxFoundation* cFoundation = NULL;
PxPhysics* cPhysics = NULL;
PxPvd* cPVD = NULL;
PxPvdTransport* cTransport = NULL;
PxCooking* cCooking = NULL;
PxScene* cScene = NULL;
PxCpuDispatcher* cDispatcher = NULL;
PxMaterial* cMaterial = NULL;
PxRigidStatic* cGroundPlane = NULL;
PxVehicleDrive4W* cVehicle4w = NULL;
VehicleSceneQueryData* cVehicleSceneQueryData = NULL;
PxBatchQuery* cBatchQuery = NULL;

std::set<int> keys; // we store key state between frames here...

bool cIsVehicleInAir = true;

static PxDefaultAllocator cDefaultAllocator;
static PxDefaultErrorCallback cErrorCallback;

std::map<physx::PxActor*, Component::ComponentId> trackedComponents;

extern VehicleDesc initVehicleDesc();

VehicleDesc initVehicleDescription() {
	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	const PxF32 chassisMass = 1500.0f;
	const PxVec3 chassisDims(2.5f, 2.0f, 5.0f);
	const PxVec3 chassisMOI
	((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) *
		chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) *
		0.8f * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) *
		chassisMass / 12.0f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.65f, 0.25f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	const PxF32 wheelMass = 20.0f;
	const PxF32 wheelRadius = 0.5f;
	const PxF32 wheelWidth = 0.4f;
	const PxF32 wheelMOI = 0.5f * wheelMass * wheelRadius * wheelRadius;
	const PxU32 nbWheels = 6;

	VehicleDesc vehicleDesc;

	vehicleDesc.chassisMass = chassisMass;
	vehicleDesc.chassisDims = chassisDims;
	vehicleDesc.chassisMOI = chassisMOI;
	vehicleDesc.chassisCMOffset = chassisCMOffset;
	vehicleDesc.chassisMaterial = cMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_CHASSIS,
		COLLISION_FLAG_CHASSIS_AGAINST,
		0, 0);

	vehicleDesc.wheelMass = wheelMass;
	vehicleDesc.wheelRadius = wheelRadius;
	vehicleDesc.wheelWidth = wheelWidth;
	vehicleDesc.wheelMOI = wheelMOI;
	vehicleDesc.numWheels = nbWheels;
	vehicleDesc.wheelMaterial = cMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL,
		COLLISION_FLAG_WHEEL_AGAINST,
		0, 0);

	return vehicleDesc;
}


void Physics::PhysicsSystem::initialize() {

	onKeyPressHandler = Engine::EventSystem::createHandler(this, &Physics::PhysicsSystem::onKeyPress);
	onKeyDownHandler = Engine::EventSystem::createHandler(this, &Physics::PhysicsSystem::onKeyDown);
    onKeyUpHandler = Engine::EventSystem::createHandler(this, &Physics::PhysicsSystem::onKeyUp);
    onVehicleCreatedHandler = Engine::EventSystem::createHandler(this, &Physics::PhysicsSystem::onVehicleCreated);

	createFoundation();
	createPVD();
	createPhysicsObject();
	createCooking();
	createScene();
	createGround();
	initVehicleSupport();
	createDrivableVehicle();

	std::cout << "Physics System Successfully Initalized" << std::endl;

}


void Physics::PhysicsSystem::createFoundation() {

	cFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, cDefaultAllocator, cErrorCallback);
	if (!cFoundation)
		Engine::assertLog<module>(false, "PxCreateFoundation failed");

}

void Physics::PhysicsSystem::createPVD() {

	cPVD = PxCreatePvd(*cFoundation);
	cTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	cPVD->connect(*cTransport, PxPvdInstrumentationFlag::eALL);

}

void Physics::PhysicsSystem::createPhysicsObject() {

	cPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *cFoundation, PxTolerancesScale(), true, cPVD);
	if (!cPhysics)
		Engine::assertLog<module>(false, "PxCreatePhysics Failed");

}

void Physics::PhysicsSystem::createCooking() {

	cCooking = PxCreateCooking(PX_PHYSICS_VERSION, *cFoundation, PxCookingParams(PxTolerancesScale()));
}

void Physics::PhysicsSystem::createScene() {

	PxSceneDesc sceneDesc(cPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.f, GRAVITY, 0.f);

	PxU32 numWorkers = 1;
	cDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	sceneDesc.cpuDispatcher = cDispatcher;
	sceneDesc.filterShader = snippetvehicle::VehicleFilterShader;

	cScene = cPhysics->createScene(sceneDesc);

}

void Physics::PhysicsSystem::createGround() {

	cMaterial = cPhysics->createMaterial(STATIC_FRICTION, DYNAMIC_FRICTION, RESTITUTION);
	PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	cGroundPlane = createDrivablePlane(groundPlaneSimFilterData, cMaterial, cPhysics);

	cScene->addActor(*cGroundPlane);

}

void Physics::PhysicsSystem::initVehicleSupport() {

	cVehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1,
		WheelSceneQueryPreFilterBlocking, nullptr,
		cDefaultAllocator);
	cBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *cVehicleSceneQueryData, cScene);

	PxInitVehicleSDK(*cPhysics);
	PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	cFrictionPairs = createFrictionPairs(cMaterial);

}

void Physics::PhysicsSystem::createDrivableVehicle() {

	VehicleDesc vehicleDesc = initVehicleDescription();

	cVehicle4w = createVehicle4W(vehicleDesc, cPhysics, cCooking);
	PxTransform startTransform(PxVec3(0, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 2.0f), -10),
		PxQuat(PxIdentity));
    cVehicle4w->getRigidDynamicActor()->setGlobalPose(startTransform);

    cScene->addActor(*cVehicle4w->getRigidDynamicActor());

    cVehicle4w->setToRestState();
    cVehicle4w->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
    cVehicle4w->mDriveDynData.setUseAutoGears(true);

}

PxVehicleDrive4W *Physics::PhysicsSystem::createDrivableVehicle(const PxTransform &worldTransform) {

    PxVehicleDrive4W *pxVehicle;
    VehicleDesc vehicleDesc = initVehicleDescription();

    pxVehicle = createVehicle4W(vehicleDesc, cPhysics, cCooking);
    PxTransform startTransform(PxVec3(0, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 2.0f), -10),
                               PxQuat(PxIdentity));

    pxVehicle->getRigidDynamicActor()->setGlobalPose(worldTransform * startTransform);

    cScene->addActor(*pxVehicle->getRigidDynamicActor());

    pxVehicle->setToRestState();
    pxVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
    pxVehicle->mDriveDynData.setUseAutoGears(true);

    return pxVehicle;
}

//CHANGE WHEN YOU HAVE MORE VEHICLES
void Physics::PhysicsSystem::stepPhysics(Engine::deltaTime timestep) {

    PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(
            cKeySmoothingData, cSteerVsForwardSpeedTable, cVehicleInputData,
            timestep, cIsVehicleInAir, *cVehicle4w);

    //raycasts
    PxVehicleWheels *vehicles[1] = {cVehicle4w};
	PxRaycastQueryResult* raycastResults = cVehicleSceneQueryData->getRaycastQueryResultBuffer(
		0);
	const PxU32 raycastResultsSize = cVehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionRaycasts(cBatchQuery, 1, vehicles, raycastResultsSize,
		raycastResults);

	//vehicle update
	const PxVec3 grav = cScene->getGravity();
	PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
	PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, cVehicle4w->mWheelsSimData.getNbWheels()} };

	PxVehicleUpdates(0.0001 + timestep / 1000.0f, grav, *cFrictionPairs, 1, vehicles, vehicleQueryResults);

	//workout if vehicle is in air
	cIsVehicleInAir = cVehicle4w->getRigidDynamicActor()->isSleeping()
		? false
		: PxVehicleIsInAir(vehicleQueryResults[0]);


	cScene->simulate(0.0001 + timestep / 1000.0f);
	cScene->fetchResults(true);

	// send the objects transform into the engine ...

	auto t = cVehicle4w->getRigidDynamicActor()->getGlobalPose();
	trackedComponents[cVehicle4w->getRigidDynamicActor()]
		.attachExistingComponent(
			Engine::createComponent<Component::PhysicsPacket>(
				glm::vec3(t.p.x, t.p.y, t.p.z),
				glm::quat(t.q.w, t.q.x, t.q.y, t.q.z))->id());

	trackedComponents[cVehicle4w->getRigidDynamicActor()]
		.attachExistingComponent(Component::Dirty::id());
}


void Physics::PhysicsSystem::update(Engine::deltaTime deltaTime) {

    // using our key states to set input data directly...
    cVehicleInputData.setDigitalAccel(keys.count(GLFW_KEY_W));
    cVehicleInputData.setDigitalBrake(keys.count(GLFW_KEY_S));
    cVehicleInputData.setDigitalSteerRight(keys.count(GLFW_KEY_A));
    cVehicleInputData.setDigitalSteerLeft(keys.count(GLFW_KEY_D));

    stepPhysics(deltaTime);
}

std::ostream& physx::operator<<(std::ostream& out, const physx::PxTransform& transform) {
	out << transform.p.x << ", " << transform.p.y << ", " << transform.p.z;
	return out;
}

void Physics::PhysicsSystem::onKeyDown(const Component::EventArgs<int>& args) {

	auto key = std::get<0>(args.values);

	Engine::log<module, Engine::low>("onKeyDown=", key);

	keys.emplace(key);
}

void Physics::PhysicsSystem::onKeyUp(const Component::EventArgs<int>& args) {

    auto key = std::get<0>(args.values);

    Engine::log<module, Engine::low>("onKeyUp=", key);

    keys.erase(key);
}

void Physics::PhysicsSystem::onKeyPress(const Component::EventArgs<int> &args) { /* do nothing */ }

void Physics::PhysicsSystem::onVehicleCreated(const Component::EventArgs<Component::ComponentId> &args) {
    auto vehicleComponent = std::get<0>(args.values);
    auto pxVehicle = createDrivableVehicle(PxTransform());

    vehicleComponent.data<Component::Vehicle>()->pxVehicle = pxVehicle;
}

void Physics::PhysicsSystem::link(Component::ComponentId sceneComponent, physx::PxActor *actor) {
    trackedComponents.emplace(actor, sceneComponent);
}

physx::PxActor *Physics::PhysicsSystem::getVehicleActor() {
    return cVehicle4w->getRigidDynamicActor();
}
