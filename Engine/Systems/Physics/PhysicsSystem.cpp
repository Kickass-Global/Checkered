//
// Created by root on 9/1/20.
//

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Vehicle.h>
#include <algorithm>
#include <PhysicsActor.h>
#include <Mesh.h>

#include "PhysicsSystem.h"
#include "PxRigidStatic.h"
#include "Events/Events.h"
#include "physicspacket.hpp"
#include "snippetvehiclecommon/SnippetVehicleTireFriction.h"
#include "tags.h"
#include "Engine.h"
#include "scenery.hpp"

using namespace physx;
using namespace snippetvehicle;

const float GRAVITY = -9.81f;
const float STATIC_FRICTION = 0.9f;
const float DYNAMIC_FRICTION = 0.5f;
const float RESTITUTION = 0.3f;

Component::Passenger* activePassenger;

namespace {
	const char module[] = "Physics";
}

std::vector<PxVehicleDrive4W*> vehicles;

PxVehicleDrivableSurfaceToTireFrictionPairs* cFrictionPairs = NULL;

PxFoundation* cFoundation = NULL;

PxPvd* cPVD = NULL;
PxPvdTransport* cTransport = NULL;
PxCooking* cCooking = NULL;
PxCpuDispatcher* cDispatcher = NULL;
PxMaterial* cMaterial = NULL;
PxRigidStatic* cGroundPlane = NULL;
PxVehicleDrive4W* cVehicle4w = NULL;
VehicleSceneQueryData* cVehicleSceneQueryData = NULL;
PxBatchQuery* cBatchQuery = NULL;

std::set<int> keys; // we store key state between frames here...

bool cIsVehicleInAir = true;
bool cIsPassengerInVehicle = false;

static PxDefaultAllocator cDefaultAllocator;
static PxDefaultErrorCallback cErrorCallback;

std::map<physx::PxRigidDynamic*, std::shared_ptr<ComponentBase>> trackedComponents;



extern VehicleDesc initVehicleDesc();


VehicleDesc initVehicleDescription() {
	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	const PxF32 chassisMass = 1500.0f;
	const PxVec3 chassisDims(2.0f, 3.0f, 4.0f);
	const PxVec3 chassisMOI
	((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) *
		chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) *
		0.8f * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) *
		chassisMass / 12.0f
	);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.05f, 0.25f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	const PxF32 wheelMass = 20.0f;
	const PxF32 wheelRadius = 0.35f;
	const PxF32 wheelWidth = 0.34f;
	const PxF32 wheelMOI = .8f * wheelMass * wheelRadius * wheelRadius;
	const PxU32 nbWheels = 4;

	VehicleDesc vehicleDesc;

	vehicleDesc.chassisMass = chassisMass;
	vehicleDesc.chassisDims = chassisDims;
	vehicleDesc.chassisMOI = chassisMOI;
	vehicleDesc.chassisCMOffset = chassisCMOffset;
	vehicleDesc.chassisMaterial = cMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(
		FilterMask::eVehicle,
		FilterMask::eEverything,
		0, 0
	);

	
	vehicleDesc.wheelMass = wheelMass;
	vehicleDesc.wheelRadius = wheelRadius;
	vehicleDesc.wheelWidth = wheelWidth;
	vehicleDesc.wheelMOI = wheelMOI;
	vehicleDesc.numWheels = nbWheels;
	vehicleDesc.wheelMaterial = cMaterial;
	vehicleDesc.wheelSimFilterData = PxFilterData(
		FilterGroup::eWheel,
		FilterMask::eWheelColliders,
		0, 0
	);

	return vehicleDesc;
}


void Physics::PhysicsSystem::initialize() {

	onKeyPressHandler = Engine::EventSystem::createHandler(this, &Physics::PhysicsSystem::onKeyPress);
	onKeyDownHandler = Engine::EventSystem::createHandler(this, &Physics::PhysicsSystem::onKeyDown);
	onKeyUpHandler = Engine::EventSystem::createHandler(this, &Physics::PhysicsSystem::onKeyUp);
	onVehicleCreatedHandler = Engine::EventSystem::createHandler(this, &Physics::PhysicsSystem::onVehicleCreated);
	onActorCreatedHandler = Engine::EventSystem::createHandler(this, &Physics::PhysicsSystem::onActorCreated);

	createFoundation();
	createPVD();
	createPhysicsObject();
	createCooking();
	createScene();
	createGround();
	initVehicleSupport();
	createDrivablePlayerVehicle();
	createPhysicsCallbacks();

	std::cout << "Physics System Successfully Initalized" << std::endl;

}

using namespace Engine;

class SimulationCallback : public PxSimulationEventCallback {

	// manually track which objects are currently overlapping.
	std::map<std::pair<size_t, size_t>, int> overlapping_actors;

	void onContact(const PxContactPairHeader& pairHeader,
		const PxContactPair* pairs, PxU32 count) override {

		for (PxU32 i = 0; i < count; i++) {
			// ignore pairs when shapes have been deleted
			if (pairs[i].flags & (PxContactPairFlag::eREMOVED_SHAPE_0 |
				PxContactPairFlag::eREMOVED_SHAPE_1))
				continue;

			//if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			//{
			//	//log<high>("Collision detected");
			//	// detect when collisions occur between objects and invoke actors events....

			//	auto a = reinterpret_cast<PhysicsActor*>(pairs[i].shapes->userData);
			//	auto b = reinterpret_cast<PhysicsActor*>(pairs[i].otherActor->userData);
			//}

		}
	}

	// Inherited via PxSimulationEventCallback
	virtual void onConstraintBreak(PxConstraintInfo * constraints, PxU32 count) override
	{
		log<high>("onConstraintBreak detected");
	}
	virtual void onWake(PxActor ** actors, PxU32 count) override
	{
		log<high>("onWake detected");
	}
	virtual void onSleep(PxActor ** actors, PxU32 count) override
	{
		log<high>("onSleep detected");
	}
	virtual void onTrigger(PxTriggerPair * pairs, PxU32 count) override
	{
		for (PxU32 i = 0; i < count; i++) {
			// ignore pairs when shapes have been deleted
			if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER |
				PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
				continue;

			if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_FOUND) {

				auto a = reinterpret_cast<PhysicsActor*>(pairs[i].triggerActor->userData);
				auto b = reinterpret_cast<PhysicsActor*>(pairs[i].otherActor->userData);

				auto[it, first_contact] = overlapping_actors.emplace(std::make_pair(reinterpret_cast<size_t>(a), reinterpret_cast<size_t>(b)), 1);
				if (first_contact) {
					if (a) a->onBeginOverlap(a, b);
					if (b) b->onBeginOverlap(b, a);
				}
				else
				{
					it->second++;
				}
			}

			if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_LOST) {

				auto a = reinterpret_cast<PhysicsActor*>(pairs[i].triggerActor->userData);
				auto b = reinterpret_cast<PhysicsActor*>(pairs[i].otherActor->userData);

				auto[it, not_overlapping] = overlapping_actors.emplace(std::make_pair(reinterpret_cast<size_t>(a), reinterpret_cast<size_t>(b)), 0);
				if (it->second <= 1) {
					if (a) a->onEndOverlap(a, b);
					if (b) b->onEndOverlap(b, a);
					overlapping_actors.erase(std::make_pair(reinterpret_cast<size_t>(a), reinterpret_cast<size_t>(b)));
				}
				else {
					it->second--;
				}

			}

		}
	}
	virtual void onAdvance(const PxRigidBody * const * bodyBuffer, const PxTransform * poseBuffer, const PxU32 count) override
	{
		log<high>("onAdvance detected");
	}
};

SimulationCallback scb;
void Physics::PhysicsSystem::createPhysicsCallbacks() {

	log<high>("Creating simulation callbacks");
	cScene->setSimulationEventCallback(&scb);
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

	PxU32 numWorkers = 4;
	cDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	sceneDesc.cpuDispatcher = cDispatcher;
	sceneDesc.filterShader = FilterShader::setupFilterShader;
	cScene = cPhysics->createScene(sceneDesc);

}

void Physics::PhysicsSystem::createGround() {

	cMaterial = cPhysics->createMaterial(STATIC_FRICTION, DYNAMIC_FRICTION, RESTITUTION);
	//PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	//cGroundPlane = createDrivablePlane(groundPlaneSimFilterData, cMaterial, cPhysics);

	//cScene->addActor(*cGroundPlane);
}

void Physics::PhysicsSystem::initVehicleSupport() {

	cVehicleSceneQueryData = VehicleSceneQueryData::allocate(
		100, PX_MAX_NB_WHEELS, 1, 100,
		WheelSceneQueryPreFilterBlocking, nullptr,
		cDefaultAllocator
	);
	cBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *cVehicleSceneQueryData, cScene);

	PxInitVehicleSDK(*cPhysics);
	PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	cFrictionPairs = createFrictionPairs(cMaterial);

}

void Physics::PhysicsSystem::createDrivablePlayerVehicle() {
	//    createDrivableVehicle(PxTransform(0,0,0));
}

//TODO DIFFERENTIATE BETWEEN ENEMY VEHICLE AND PLAYER VEHICLE FOR COLLIDERS
PxVehicleDrive4W* Physics::PhysicsSystem::createDrivableVehicle(const PxTransform& worldTransform) {

	PxVehicleDrive4W* pxVehicle;
	VehicleDesc vehicleDesc = initVehicleDescription();


	vehicles.push_back(createVehicle4W(vehicleDesc, cPhysics, cCooking));
	pxVehicle = vehicles.back();

	PxTransform startTransform(
		PxVec3(0, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 2.0f), -10),
		PxQuat(PxIdentity));

	pxVehicle->getRigidDynamicActor()->setGlobalPose(startTransform * worldTransform);

	cScene->addActor(*pxVehicle->getRigidDynamicActor());

	pxVehicle->setToRestState();
	pxVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	pxVehicle->mDriveDynData.setUseAutoGears(true);

	PxVehicleEngineData engine;

	engine.mMOI = 1;
	engine.mPeakTorque = 10000.0;
	engine.mMaxOmega = 3600.0;
	engine.mDampingRateFullThrottle = 0.0095;
	engine.mDampingRateZeroThrottleClutchEngaged = 0.0040;
	engine.mDampingRateZeroThrottleClutchDisengaged = 0.0035;
	

	pxVehicle->mDriveSimData.setEngineData(engine);

	PxVehicleTireData tireData;
	tireData.mFrictionVsSlipGraph[0][0] = 0.f;
	tireData.mFrictionVsSlipGraph[0][1] = 1.f;
	tireData.mFrictionVsSlipGraph[1][0] = 0.5f;
	tireData.mFrictionVsSlipGraph[1][1] = 1.0f;
	tireData.mFrictionVsSlipGraph[2][0] = 1.f;
	tireData.mFrictionVsSlipGraph[2][1] = 1.f;
	tireData.mLongitudinalStiffnessPerUnitGravity = 110.f;
	tireData.mLatStiffX = 3;
	tireData.mLatStiffY = 19;

	for (int i = 0; i < 3; i++) {
		pxVehicle->mWheelsSimData.setTireData(0, tireData);

	}

	PxVehicleAckermannGeometryData acker;
	acker.mAccuracy = 1.0f;
	acker.mFrontWidth = 1.0f;
	acker.mRearWidth = 1.0f;
	acker.mAxleSeparation = 1.0f;
	pxVehicle->mDriveSimData.setAckermannGeometryData(acker);

	PxVehicleClutchData clutch;

	clutch.mStrength = 40.0;
	clutch.mAccuracyMode = PxVehicleClutchAccuracyMode::eESTIMATE;
	clutch.mEstimateIterations = 5.0;

	pxVehicle->mDriveSimData.setClutchData(clutch);


	

	PxVehicleGearsData gears;
	pxVehicle->mDriveSimData.setGearsData(gears);


	return pxVehicle;
}

void Physics::PhysicsSystem::stepPhysics(Engine::deltaTime timestep) {

	// update all vehicles in the scene.
	auto vehicles = Engine::getStore().getRoot().getComponentsOfType<Component::Vehicle>();
	std::vector<Component::Vehicle*> active;

	std::copy_if(
		vehicles.begin(), vehicles.end(), std::back_inserter(active), [](Component::Vehicle* vehicle) {
		return vehicle->pxVehicle; // vehicles might not be initialized yet...
	}
	);

	for (auto& meta : active) {

		PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(
			meta->pxPadSmoothingData, meta->pxSteerVsForwardSpeedTable, meta->pxVehicleInputData,
			timestep, meta->pxIsVehicleInAir, *meta->pxVehicle
		);

	}

	std::vector<PxVehicleWheels*> wheels;
	std::transform(active.begin(), active.end(), std::back_inserter(wheels), [](auto meta) { return meta->pxVehicle; });

	if (!wheels.empty()) {
		//raycasts
		PxRaycastQueryResult* raycastResults = cVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
		const PxU32 raycastResultsSize = cVehicleSceneQueryData->getQueryResultBufferSize();
		PxVehicleSuspensionRaycasts(
			cBatchQuery, wheels.size(), wheels.data(), raycastResultsSize, raycastResults
		);

		//vehicle update
		const PxVec3 grav = cScene->getGravity();
		PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];

		std::vector<PxVehicleWheelQueryResult> vehicleQueryResults;
		for (auto w : wheels) {

			vehicleQueryResults.push_back({ wheelQueryResults, wheels[0]->mWheelsSimData.getNbWheels() });
		}
		PxVehicleUpdates(
			0.0001 + timestep / 1000.0f, grav, *cFrictionPairs, wheels.size(), wheels.data(),
			vehicleQueryResults.data());
	}

	cScene->simulate(0.0001 + timestep / 1000.0f);
	cScene->fetchResults(true);

	// replicate physx bodies' world transforms to corresponding components.
	for (auto&[actor, component] : trackedComponents) {
		auto t = actor->getGlobalPose();

		component->emplaceChildComponent<Component::PhysicsPacket>(
			glm::vec3(t.p.x, t.p.y, t.p.z),
			glm::quat(t.q.w, t.q.x, t.q.y, t.q.z)
			);
	}
}


void Physics::PhysicsSystem::update(Engine::deltaTime deltaTime) {
	deltaTime = std::min(deltaTime, 32.0f);
	const auto step_target = 8.0;
	int steps = std::floor(deltaTime / step_target);
	float step_delta = deltaTime / steps;
	for (auto step = 0; step < steps; ++step)
	{
		stepPhysics(step_delta);
		
	}
	
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

void Physics::PhysicsSystem::onKeyPress(const Component::EventArgs<int>& args) { /* do nothing */ }

void Physics::PhysicsSystem::onVehicleCreated(const Component::EventArgs<Component::Vehicle*>& args) {

	const auto& vehicleComponent = std::get<0>(args.values);

	// grab the vehicle world position and set the physx actors transform accordingly.

	auto T = PxTransform(
		vehicleComponent->position.x,
		vehicleComponent->position.y,
		vehicleComponent->position.z
	);


	auto pxVehicle = createDrivableVehicle(T);

	if (vehicleComponent->type == Vehicle::Type::Player)
	{
		//FilterShader::setupFiltering(pxVehicle->getRigidDynamicActor(), FilterGroup::ePlayerVehicle, FilterMask::eVehicleColliders );
		//FilterShader::setupQueryFiltering(pxVehicle->getRigidDynamicActor(), 0, QueryFilterMask::eDrivable);
	}
	else {
		//FilterShader::setupFiltering(pxVehicle->getRigidDynamicActor(), FilterGroup::eEnemyVehicle, FilterMask::eVehicleColliders);
		//FilterShader::setupQueryFiltering(pxVehicle->getRigidDynamicActor(), 0, QueryFilterMask::eDrivable);
	}

	Engine::log<module, Engine::high>("onVehicleCreated #", vehicleComponent);

	// link the component with the physx actor so we can replicate updates.
	vehicleComponent->pxVehicle = pxVehicle;
	link(vehicleComponent, pxVehicle->getRigidDynamicActor());
}

PxTriangleMesh* Physics::PhysicsSystem::createTriMesh(Mesh* mesh) {

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = mesh->vertices.size();
	meshDesc.points.stride = sizeof(Vertex);
	meshDesc.points.data = mesh->vertices.data();

	meshDesc.triangles.count = mesh->indices.size() / 3; // assumption tri-mesh
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = mesh->indices.data();

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = cCooking->cookTriangleMesh(meshDesc, writeBuffer, &result);

	Engine::assertLog(status, "Cooking triangle mesh");

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

	return cPhysics->createTriangleMesh(readBuffer);
}

PxConvexMesh* Physics::PhysicsSystem::createConvexMesh(Mesh* mesh) {

	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = mesh->vertices.size();
	convexDesc.points.stride = sizeof(Vertex);
	convexDesc.points.data = mesh->vertices.data();
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	PxDefaultMemoryOutputStream buf;
	PxConvexMeshCookingResult::Enum result;
	auto status = cCooking->cookConvexMesh(convexDesc, buf, &result);

	Engine::assertLog(status, "Cooking convex mesh");

	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	return cPhysics->createConvexMesh(input);
}


void Physics::PhysicsSystem::onActorCreated(const Component::EventArgs<Component::PhysicsActor*>& args) {

	Engine::log<module>("Running onActorCreated");

	auto& aPhysicsActor = std::get<0>(args.values);
	auto& aMesh = aPhysicsActor->mesh;

	auto position = physx::PxVec3{ aPhysicsActor->position.x, aPhysicsActor->position.y, aPhysicsActor->position.z };
	auto rotation = physx::PxQuat{ aPhysicsActor->rotation.x, aPhysicsActor->rotation.y, aPhysicsActor->rotation.z, aPhysicsActor->rotation.w };


	if (aPhysicsActor->type == PhysicsActor::Type::StaticObject) {
		aPhysicsActor->actor = cPhysics->createRigidStatic(PxTransform(position, rotation));
		PxShape* aConvexShape = PxRigidActorExt::createExclusiveShape(*aPhysicsActor->actor,
			PxConvexMeshGeometry(createConvexMesh(aMesh.get())),
			*cMaterial
		);
	}
	if (aPhysicsActor->type == PhysicsActor::Type::Ground) {
		aPhysicsActor->actor = cPhysics->createRigidStatic(PxTransform(position, rotation));
		PxShape* aConvexShape = PxRigidActorExt::createExclusiveShape(*aPhysicsActor->actor,
			PxTriangleMeshGeometry(createTriMesh(aMesh.get())),
			*cMaterial
		);
	}

	if (aPhysicsActor->type == PhysicsActor::Type::TriggerVolume) {
		aPhysicsActor->actor = cPhysics->createRigidStatic(PxTransform(position, rotation));
		PxShape* aConvexShape = PxRigidActorExt::createExclusiveShape(*aPhysicsActor->actor,
			PxConvexMeshGeometry(createConvexMesh(aMesh.get())),
			*cMaterial, (PxShapeFlag::eTRIGGER_SHAPE | PxShapeFlag::eVISUALIZATION)
		);
	}

	if (aPhysicsActor->type == PhysicsActor::Type::DynamicObject) {
		auto rigid = cPhysics->createRigidDynamic(PxTransform(position, rotation));
		rigid->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
		aPhysicsActor->actor = rigid;
		PxShape* aConvexShape = PxRigidActorExt::createExclusiveShape(*aPhysicsActor->actor,
			PxConvexMeshGeometry(createConvexMesh(aMesh.get())),
			*cMaterial
		);
	}

	aPhysicsActor->actor->userData = aPhysicsActor;

	switch (aPhysicsActor->type)
	{
	case PhysicsActor::Type::StaticObject:
		FilterShader::setupFiltering(aPhysicsActor->actor, FilterGroup::eScenery, FilterMask::eEverything);
		break;
	case PhysicsActor::Type::DynamicObject:
		FilterShader::setupFiltering(aPhysicsActor->actor, FilterGroup::eObstacle, FilterMask::eEverything);
		break;
	case PhysicsActor::Type::Ground:
		FilterShader::setupFiltering(aPhysicsActor->actor, FilterGroup::eGround, FilterMask::eGroundColliders);
		FilterShader::setupQueryFiltering(aPhysicsActor->actor, 0, QueryFilterMask::eDrivable);
		break;
	case PhysicsActor::Type::TriggerVolume:
		FilterShader::setupFiltering(aPhysicsActor->actor, FilterGroup::eTrigger, 0);
		break;
	}
	cScene->addActor(*aPhysicsActor->actor);
}

void Physics::PhysicsSystem::onPassengerCreated(Component::Passenger* passenger) {

	auto passengers = Engine::getStore().getRoot().getComponentsOfType<Component::Passenger>();
	for (auto passenger : passengers) {

		if (!passenger->pickup_actor->actor->actor || !passenger->dropoff_actor->actor->actor) continue; // todo, not this....

		PxRigidStatic* temp_rigstat_dropoff = cPhysics->createRigidStatic(passenger->dropOffTransform);

		passenger->pass_material = cPhysics->createMaterial(100.0f, 100.f, 100.f);

		PxShapeFlags pass_flags = (PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eVISUALIZATION);

		activePassenger = passenger;

		break; // only make one at a time?
	}
}

Component::Passenger* Physics::PhysicsSystem::createPassenger(const PxTransform& pickupTrans, const PxTransform& dropOffTrans) {
	//create temp passenger component
	Component::Passenger* temp_pass = nullptr;

	return(temp_pass);

}

void Physics::PhysicsSystem::link(Vehicle* sceneComponent, physx::PxRigidDynamic* actor) {
	trackedComponents.emplace(actor, sceneComponent);
}






