
#ifndef PASSENGER_H
#define PASSENGER_H

#include "Component.h"
#include "ComponentId.h"
#include "Engine.h"
#include "ReportCard.h"
#include "scenery.hpp"
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include <PxRigidActor.h>
#include <PxRigidBody.h>
#include <PxShape.h>
#include "Sound.h"

using namespace physx;

namespace Component {

class Passenger : public ComponentBase {

public:
  std::shared_ptr<Waypoint> pickup_actor;
  std::shared_ptr<Waypoint> dropoff_actor;
  EventDelegate<int> onPassengerPickedUpDelegate{""};
  EventDelegate<int> onPassengerDroppedOffDelegate{""};

  PxPhysics *pass_physics;
  PxMaterial *pass_material;

  PxTransform pickupTransform;
  PxTransform dropOffTransform;

  ReportCard passengerReportCard;

  void setPickupTransform(PxTransform pickupTrans);
  void setDropoffTransform(PxTransform dropoffTrans);

  Passenger(glm::vec3 pickup_location, glm::vec3 dropoff_location,
            std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) {

    pickup_actor =
        getEngine()->createComponent<Waypoint>(pickup_location, mesh, material);
    dropoff_actor = getEngine()->createComponent<Waypoint>(dropoff_location,
                                                           mesh, material);
    pickup_actor->onEntered +=
        std::bind(&Passenger::onPassengerPickedUp, this, std::placeholders::_1);
    dropoff_actor->onEntered += std::bind(&Passenger::onPassengerDroppedOff,
                                          this, std::placeholders::_1);
    dropoff_actor->mesh->enabled = false;
  }

private:
  void onPassengerPickedUp(PhysicsActor *) {
    using namespace Engine;
    log<high>("Passenger picked up");
    dropoff_actor->mesh->enabled = true;
    pickup_actor->mesh->enabled = false;
    onPassengerPickedUpDelegate(0);
  }

  void onPassengerDroppedOff(PhysicsActor *) {
    using namespace Engine;
    log<high>("Passenger dropped off");
    pickup_actor->mesh->enabled = false;
    dropoff_actor->mesh->enabled = false;
    onPassengerDroppedOffDelegate(0);
    getEngine()->createComponent<Component::Sound>("passengerDroppedOff");
  }
};

} // namespace Component
#endif
