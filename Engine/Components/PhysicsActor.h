//
// Created by Jackson Cougar Wiebe on 3/1/2020.
//

#ifndef ENGINE_PHYSICSACTOR_H
#define ENGINE_PHYSICSACTOR_H

#include <Component.h>
#include <PxPhysicsAPI.h>
#include "Mesh.h"
#include <memory>

namespace Component {
    class PhysicsActor : public ComponentBase {

    public:
        std::shared_ptr<Mesh> mesh;
        physx::PxRigidActor *actor;

        explicit PhysicsActor(std::shared_ptr<Component::Mesh> &mesh);
    };
}


#endif //ENGINE_PHYSICSACTOR_H
