#pragma once

#include <PxPhysicsAPI.h>
#include <Physics/PhysicsSystem.h>
#include <PxSimulationEventCallback.h>
#include <Passenger.h>

using namespace physx;

namespace Engine {
    class SimulationCallback : public PxSimulationEventCallback {

        // manually track which objects are currently overlapping.
        std::map<std::pair<size_t, size_t>, int> overlapping_actors;

        void onContact(
            const PxContactPairHeader &pairHeader, const PxContactPair *pairs, PxU32 count
        ) override;

        // Inherited via PxSimulationEventCallback
        void onConstraintBreak(PxConstraintInfo *constraints, PxU32 count) override;

        void onWake(PxActor **actors, PxU32 count) override;

        void onSleep(PxActor **actors, PxU32 count) override;

        void onTrigger(PxTriggerPair *pairs, PxU32 count) override;

        void onAdvance(
            const PxRigidBody *const *bodyBuffer, const PxTransform *poseBuffer, const PxU32 count
        ) override;
    };
}
