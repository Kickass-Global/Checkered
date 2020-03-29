#include "SimulationCallback.h"
#include "PhysicsActor.h"

void Engine::SimulationCallback::onContact(
    const PxContactPairHeader &pairHeader, const PxContactPair *pairs,
    PxU32 count) {
  const PxU32 bufferSize = 64;
  PxContactPairPoint contacts[bufferSize];
  for (PxU32 i = 0; i < count; i++) {
    // ignore pairs when shapes have been deleted
    //      if (pairs[i].flags & (PxContactPairFlag::eREMOVED_SHAPE_0 |
    //                            PxContactPairFlag::eREMOVED_SHAPE_1))
    //        continue;

    if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
      log<Physics::module, low>("Collision detected");
      //         detect when collisions occur between objects and invoke
      //         actors events....
      auto a =
          reinterpret_cast<ComponentBase *>(pairHeader.actors[0]->userData);
      auto b =
          reinterpret_cast<ComponentBase *>(pairHeader.actors[1]->userData);

      auto a_actor = dynamic_cast<PhysicsActor *>(a);
      auto b_actor = dynamic_cast<PhysicsActor *>(b);
      auto a_vehicle = dynamic_cast<Vehicle *>(a);
      auto b_vehicle = dynamic_cast<Vehicle *>(b);
      if (a_actor && b_actor) {
        a_actor->onHit(a_actor, b_actor);
        b_actor->onHit(b_actor, a_actor);
      }
      //      if (a_vehicle && b_actor) {
      //        a->getStore().emplaceComponent<Component::CollisionEvent,
      //        1>(b_actor);
      //        b->getStore().emplaceComponent<Component::CollisionEvent,
      //        1>(a_actor);
      //      }
      //      if (b_vehicle && a_actor) {
      //        a->getStore().emplaceComponent<Component::CollisionEvent,
      //        1>(b_actor);
      //        b->getStore().emplaceComponent<Component::CollisionEvent,
      //        1>(a_actor);
      //      }
      if (a_vehicle && b_vehicle) {

        PxU32 nbContacts = pairs[i].extractContacts(contacts, bufferSize);
        PxVec3 total_force{0,0,0};
        for(PxU32 j=0; j < nbContacts; j++)
        {
          total_force += contacts[j].impulse;
        }
        a_vehicle->onHit(b_vehicle, total_force.magnitude());
        b_vehicle->onHit(a_vehicle, total_force.magnitude());
      }
    }
  }
}

void Engine::SimulationCallback::onConstraintBreak(
    PxConstraintInfo *constraints, PxU32 count) {

  log<high>("onConstraintBreak detected");
}

void Engine::SimulationCallback::onWake(PxActor **actors, PxU32 count) {

  log<high>("onWake detected");
}

void Engine::SimulationCallback::onSleep(PxActor **actors, PxU32 count) {

  log<high>("onSleep detected");
}

void Engine::SimulationCallback::onTrigger(PxTriggerPair *pairs, PxU32 count) {

  for (PxU32 i = 0; i < count; i++) {
    // ignore pairs when shapes have been deleted
    if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER |
                          PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
      continue;

    if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_FOUND) {

      auto a = dynamic_cast<PhysicsActor *>(
          static_cast<ComponentBase *>(pairs[i].triggerActor->userData));
      auto b = dynamic_cast<PhysicsActor *>(
          static_cast<ComponentBase *>(pairs[i].otherActor->userData));

      auto [it, first_contact] = overlapping_actors.emplace(
          std::make_pair(reinterpret_cast<size_t>(a),
                         reinterpret_cast<size_t>(b)),
          1);
      if (first_contact) {
        if (a)
          a->onBeginOverlap(a, b);
        if (b)
          b->onBeginOverlap(b, a);
      } else {
        it->second++;
      }
    }

    if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_LOST) {

      auto a = dynamic_cast<PhysicsActor *>(
          static_cast<ComponentBase *>(pairs[i].triggerActor->userData));
      auto b = dynamic_cast<PhysicsActor *>(
          static_cast<ComponentBase *>(pairs[i].otherActor->userData));

      auto [it, not_overlapping] = overlapping_actors.emplace(
          std::make_pair(reinterpret_cast<size_t>(a),
                         reinterpret_cast<size_t>(b)),
          0);
      if (it->second <= 1) {
        if (a)
          a->onEndOverlap(a, b);
        if (b)
          b->onEndOverlap(b, a);
        overlapping_actors.erase(std::make_pair(reinterpret_cast<size_t>(a),
                                                reinterpret_cast<size_t>(b)));
      } else {
        it->second--;
      }
    }
  }
}

void Engine::SimulationCallback::onAdvance(const PxRigidBody *const *bodyBuffer,
                                           const PxTransform *poseBuffer,
                                           const PxU32 count) {

  log<high>("onAdvance detected");
}
