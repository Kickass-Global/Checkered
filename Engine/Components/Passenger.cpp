

#include "Passenger.h"

using namespace physx;


void Component::Passenger::initPassenger() {
	//create material for passenger
	pass_material = pass_physics->createMaterial(PASS_STATIC_FRICTION, PASS_DYNAMIC_FRICTION, PASS_RESTITUTION);
	if (!pass_material)
		Engine::assertLog<module>(false, "Passenger Material failed to create");



	//create physics shape and bind to physics actor
	PxShapeFlags shapeFlags = (PxShapeFlag::eTRIGGER_SHAPE | PxShapeFlag::eVISUALIZATION);
	pass_shape = pass_physics->createShape(PxSphereGeometry(1.0f), &pass_material, true,
		shapeFlags);
	pass_actor->attachShape(*pass_shape);
	pass_shape->release();

}
