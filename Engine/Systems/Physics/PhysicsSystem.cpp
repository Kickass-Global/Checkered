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

using namespace physx;
using namespace snippetvehicle;

const float GRAVITY = -9.81f;
const float STATIC_FRICTION = 1.9f;
const float DYNAMIC_FRICTION = 2.9f;
const float RESTITUTION = 0.1f;

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

static PxDefaultAllocator cDefaultAllocator;
static PxDefaultErrorCallback cErrorCallback;

std::map<physx::PxRigidDynamic*, std::shared_ptr<ComponentBase>> trackedComponents;

struct FliterGroup {
	enum Enum
	{
		ePlayerVehicle = (1 << 0),
		eEnemyVehicle = (1 << 1),
		ePasenger = (1 << 2)
	};
};

extern VehicleDesc initVehicleDesc();


VehicleDesc initVehicleDescription() {
	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	const PxF32 chassisMass = 1500.0f;
	const PxVec3 chassisDims(3.0f, 2.0f, 5.0f);
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
	const PxF32 wheelRadius = 0.25f;
	const PxF32 wheelWidth = 0.24f;
	const PxF32 wheelMOI = 0.5f * wheelMass * wheelRadius * wheelRadius;
	const PxU32 nbWheels = 6;

	VehicleDesc vehicleDesc;

	vehicleDesc.chassisMass = chassisMass;
	vehicleDesc.chassisDims = chassisDims;
	vehicleDesc.chassisMOI = chassisMOI;
	vehicleDesc.chassisCMOffset = chassisCMOffset;
	vehicleDesc.chassisMaterial = cMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(
		COLLISION_FLAG_CHASSIS,
		COLLISION_FLAG_CHASSIS_AGAINST,
		0, 0
	);

	vehicleDesc.wheelMass = wheelMass;
	vehicleDesc.wheelRadius = wheelRadius;
	vehicleDesc.wheelWidth = wheelWidth;
	vehicleDesc.wheelMOI = wheelMOI;
	vehicleDesc.numWheels = nbWheels;
	vehicleDesc.wheelMaterial = cMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(
		COLLISION_FLAG_WHEEL,
		COLLISION_FLAG_WHEEL_AGAINST,
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

void Physics::PhysicsSystem::createPhysicsCallbacks() {



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
	sceneDesc.filterShader = snippetvehicle::VehicleFilterShader;

	cScene = cPhysics->createScene(sceneDesc);

}

void Physics::PhysicsSystem::createGround() {

	cMaterial = cPhysics->createMaterial(STATIC_FRICTION, DYNAMIC_FRICTION, RESTITUTION);
	PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	cGroundPlane = createDrivablePlane(groundPlaneSimFilterData, cMaterial, cPhysics);

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
	
	pxVehicle->mDriveSimData.setEngineData(engine);

	PxVehicleClutchData clutch;
	pxVehicle->mDriveSimData.setClutchData(clutch);

	PxVehicleGearsData gears;
	pxVehicle->mDriveSimData.setGearsData(gears);



	FilterShader::setupFiltering(pxVehicle->getRigidDynamicActor(), FliterGroup::ePlayerVehicle, FliterGroup::ePasenger);



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
			cBatchQuery, wheels.size(), wheels.data(), raycastResultsSize,
			raycastResults
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

		//workout if vehicle is in air
		//cIsVehicleInAir = meta->pxVehicle->getRigidDynamicActor()->isSleeping()
		//	? false
		//	: PxVehicleIsInAir(vehicleQueryResults[0]);
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

	for (const auto& actor : Engine::getStore().getRoot().getComponentsOfType<Component::PhysicsActor>()) {
		//
	}

	if (playerVehicle) {
		//// using our key states to set input data directly...
		//playerVehicle->pxVehicleInputData.setDigitalAccel(keys.count(GLFW_KEY_W));
		//playerVehicle->pxVehicleInputData.setDigitalBrake(keys.count(GLFW_KEY_S));
		//playerVehicle->pxVehicleInputData.setDigitalSteerRight(keys.count(GLFW_KEY_A));
		//playerVehicle->pxVehicleInputData.setDigitalSteerLeft(keys.count(GLFW_KEY_D));
	}
	stepPhysics(std::min(deltaTime, 32.0f));
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

	Engine::log<module, Engine::high>("onVehicleCreated #", vehicleComponent);

	// link the component with the physx actor so we can replicate updates.
	vehicleComponent->pxVehicle = pxVehicle;
	link(vehicleComponent, pxVehicle->getRigidDynamicActor());
}

void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask)
{
	PxFilterData filterData;
	filterData.word0 = filterGroup; // word0 = own ID
	filterData.word1 = filterMask;  // word1 = ID mask to filter pairs that trigger a
									// contact callback;
	const PxU32 numShapes = actor->getNbShapes();

	std::vector < PxShape* >shapes;
	shapes.resize(numShapes);

	actor->getShapes(shapes.data(), numShapes);
	PxFilterData qryFilterData;
	setupDrivableSurface(qryFilterData);
	for (PxU32 i = 0; i < numShapes; i++)
	{
		PxShape* shape = shapes[i];
		shape->setQueryFilterData(qryFilterData);
		shape->setSimulationFilterData(filterData);
	}
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

	aPhysicsActor->actor = cPhysics->createRigidStatic(PxTransform(position, rotation));

	if (aPhysicsActor->type == PhysicsActor::Type::StaticObject) {
		PxShape* aConvexShape = PxRigidActorExt::createExclusiveShape(*aPhysicsActor->actor,
			PxConvexMeshGeometry(createConvexMesh(aMesh.get())),
			*cMaterial
		);
	}
	if (aPhysicsActor->type == PhysicsActor::Type::Ground) {
		PxShape* aConvexShape = PxRigidActorExt::createExclusiveShape(*aPhysicsActor->actor,
			PxTriangleMeshGeometry(createTriMesh(aMesh.get())),
			*cMaterial
		);
	}

	switch (aPhysicsActor->type)
	{
	case PhysicsActor::Type::StaticObject:
		setupFiltering(aPhysicsActor->actor, COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST);
		break;
	case PhysicsActor::Type::DynamicObject:
		// todo
		break;
	case PhysicsActor::Type::Ground:
		setupFiltering(aPhysicsActor->actor, COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST);
		break;
	}
	cScene->addActor(*aPhysicsActor->actor);
}

#include <scenery.hpp>
void Physics::PhysicsSystem::onPassengerCreated(Component::Passenger* passenger) {

	auto passengers = Engine::getStore().getRoot().getComponentsOfType<Component::Passenger>();
	for (auto passenger : passengers) {
		
		if (!passenger->pickup_actor->actor->actor || !passenger->dropoff_actor->actor->actor) continue; // todo, not this....
		
		PxRigidStatic* temp_rigstat_dropoff = cPhysics->createRigidStatic(passenger->dropOffTransform);

		passenger->pass_material = cPhysics->createMaterial(100.0f, 100.f, 100.f);

		PxShapeFlags pass_flags = (PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eVISUALIZATION);

		//PxShape* pass_shape_pickup = cPhysics->createShape(PxSphereGeometry(1.0f), *passenger->pass_material, true, pass_flags);
		//passenger->pickup_actor->actor->actor->attachShape(*pass_shape_pickup);
		//pass_shape_pickup->release();

		//PxShape* pass_shape_dropoff = cPhysics->createShape(PxSphereGeometry(1.f), *passenger->pass_material, true, pass_flags);
		//passenger->dropoff_actor->actor->actor->attachShape(*pass_shape_dropoff);
		//pass_shape_dropoff->release();

		//cScene->addActor(*passenger->pickup_actor->actor->actor);
		//cScene->addActor(*passenger->dropoff_actor->actor->actor);

		activePassenger = passenger;

		break; // only make one at a time?
	}
}

Component::Passenger* Physics::PhysicsSystem::createPassenger(const PxTransform& pickupTrans, const PxTransform& dropOffTrans) {
	//create temp passenger component
	Component::Passenger* temp_pass = nullptr;

	////create material for passenger with maximum friction (probably not actually necessary
	//temp_pass->pass_material = cPhysics->createMaterial(100.0f, 100.f, 100.f);

	//PxShapeFlags pass_flags = (PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eVISUALIZATION);

	//PxShape* pass_shape_pickup = cPhysics->createShape(PxSphereGeometry(1.0f), *temp_pass->pass_material, true, pass_flags);
	//temp_pass->pass_actor_pickup->attachShape(*pass_shape_pickup);
	//pass_shape_pickup->release();

	//PxShape* pass_shape_dropoff = cPhysics->createShape(PxSphereGeometry(1.f), *temp_pass->pass_material, true, pass_flags);
	//temp_pass->pass_actor_dropoff->attachShape(*pass_shape_dropoff);
	//pass_shape_dropoff->release();

	return(temp_pass);

}

void Physics::PhysicsSystem::link(Vehicle* sceneComponent, physx::PxRigidDynamic* actor) {
	trackedComponents.emplace(actor, sceneComponent);
}






