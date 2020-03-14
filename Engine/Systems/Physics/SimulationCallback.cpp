#include "SimulationCallback.h"
#include "PhysicsActor.h"

void Engine::SimulationCallback::onContact(
    const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 count
) {

    for (PxU32 i = 0; i < count; i++) {
        // ignore pairs when shapes have been deleted
        if (pairs[i].flags & (PxContactPairFlag::eREMOVED_SHAPE_0 | PxContactPairFlag::eREMOVED_SHAPE_1))
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

void Engine::SimulationCallback::onConstraintBreak(PxConstraintInfo *constraints, PxU32 count) {

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
        if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
            continue;

        if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_FOUND) {

            auto a = dynamic_cast<PhysicsActor *>(static_cast<ComponentBase *>(pairs[i].triggerActor->userData));
            auto b = dynamic_cast<PhysicsActor *>(static_cast<ComponentBase *>(pairs[i].otherActor->userData));

            auto[it, first_contact] = overlapping_actors.emplace(
                std::make_pair(reinterpret_cast<size_t>(a), reinterpret_cast<size_t>(b)), 1
            );
            if (first_contact) {
                if (a) a->onBeginOverlap(a, b);
                if (b) b->onBeginOverlap(b, a);
            } else {
                it->second++;
            }
        }

        if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_LOST) {

            auto a = dynamic_cast<PhysicsActor *>(static_cast<ComponentBase *>(pairs[i].triggerActor->userData));
            auto b = dynamic_cast<PhysicsActor *>(static_cast<ComponentBase *>(pairs[i].otherActor->userData));

            auto[it, not_overlapping] = overlapping_actors.emplace(
                std::make_pair(
                    reinterpret_cast<size_t>(a), reinterpret_cast<size_t>(b)), 0
            );
            if (it->second <= 1) {
                if (a) a->onEndOverlap(a, b);
                if (b) b->onEndOverlap(b, a);
                overlapping_actors.erase(
                    std::make_pair(
                        reinterpret_cast<size_t>(a), reinterpret_cast<size_t>(b)));
            } else {
                it->second--;
            }

        }

    }
}

void Engine::SimulationCallback::onAdvance(
    const PxRigidBody *const *bodyBuffer, const PxTransform *poseBuffer, const PxU32 count
) {

    log<high>("onAdvance detected");
}
