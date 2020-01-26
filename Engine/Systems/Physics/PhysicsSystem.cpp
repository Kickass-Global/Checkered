//
// Created by root on 9/1/20.
//

#include <assert.h>
#include <iostream>
#include "PhysicsSystem.h"


using namespace physx;

PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;
PxPvd* gPVD = NULL;
PxPvdTransport* gTransport = NULL;

void Physics::PhysicsSystem::initialize() {

	createFoundation();

	createPVD();

	createPhysicsObject();

	std::cout << "Physics System Successfully Initalized" << std::endl;

}


void Physics::PhysicsSystem::createFoundation() {
	static PxDefaultAllocator gDefaultAllocator;
	static PxDefaultErrorCallback gErrorCallback;

	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocator, gErrorCallback);
	if (!gFoundation)
		assert(false, "PxCreateFoundation failed");

}

void Physics::PhysicsSystem::createPVD() {

	gPVD = PxCreatePvd(*gFoundation);
	gTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	gPVD->connect(*gTransport, PxPvdInstrumentationFlag::eALL);

}

void Physics::PhysicsSystem::createPhysicsObject() {

	auto gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPVD);
	if (!gPhysics)
		assert(false, "PxCreatePhysics Failed");

}