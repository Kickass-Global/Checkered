
#ifndef PASSENGER_H
#define PASSENGER_H

#include "Component.h"
#include "ComponentId.h"
#include "Engine.h"
#include "ReportCard.h"
#include "Sound.h"
#include "scenery.hpp"
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include <PxRigidActor.h>
#include <PxRigidBody.h>
#include <PxShape.h>
#include <future>
#include <thread>

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

  std::shared_ptr<ReportCard> passengerReportCard;

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

    getEngine()->getSubSystem<EngineStore>()->getRoot().deactivate<Waypoint>(
        dropoff_actor.get());

    passengerReportCard = getEngine()->createComponent<Component::ReportCard>();

    passengerReportCard->setReportCardGradeTimes(20, 30, 45, 60, 120);
  }

private:
  void onPassengerPickedUp(PhysicsActor *) {
    using namespace Engine;
    log<high>("Passenger picked up");

    passengerReportCard->startReportCardTimer();

    getEngine()->getSubSystem<EngineStore>()->getRoot().deactivate<Waypoint>(
        pickup_actor.get());
    getEngine()->getSubSystem<EngineStore>()->getRoot().activate<Waypoint>(
        dropoff_actor.get());
    dropoff_actor->mesh->enabled = true;
    pickup_actor->mesh->enabled = false;
    onPassengerPickedUpDelegate(0);
  }

  void onPassengerDroppedOff(PhysicsActor *) {
    using namespace Engine;
    log<high>("Passenger dropped off");

    passengerReportCard->endReportCardTimer();

    std::thread([&]() {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        passengerReportCard->destroyReportCard();
    }).detach(); // todo not this...

    getEngine()->getSubSystem<EngineStore>()->getRoot().deactivate<Waypoint>(
        dropoff_actor.get());
    pickup_actor->mesh->enabled = false;
    dropoff_actor->mesh->enabled = false;
    onPassengerDroppedOffDelegate(0);
    getEngine()->createComponent<Component::Sound>("passengerDroppedOff");
  }
};

struct PassengerSystem : public SystemInterface {
  /*
  std::vector<glm::vec3> locations = { {0,0,0},{106,0,-30},
  {212,-13.5,239},{-151.5,0,43},{-117,21.84,-336} };
  */

  std::vector<glm::vec3> locations = {{-4, 0, -40},
                                      {-20, 0, -35}}; // possible spawns
  std::shared_ptr<Passenger> current_passenger;

  void update(Engine::deltaTime elapsed) override;
  void initialize() override;
};

} // namespace Component
#endif
