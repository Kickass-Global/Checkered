

#include "Passenger.h"

using namespace physx;



void Component::Passenger::setPickupTransform(PxTransform trans) {
	pickupTransform = trans;
}

void Component::Passenger::setDropoffTransform(PxTransform trans) {
	dropOffTransform = trans;
}