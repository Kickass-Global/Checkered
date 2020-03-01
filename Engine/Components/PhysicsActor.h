//
// Created by Jackson Cougar Wiebe on 3/1/2020.
//

#ifndef ENGINE_PHYSICSACTOR_H
#define ENGINE_PHYSICSACTOR_H

#include <Component.h>
#include <PxPhysicsAPI.h>
#include "Mesh.h"

namespace Component {
    class PhysicsActor : public ComponentBase<ClassId::PhysicsActor> {

    public:
        Mesh *mesh;
        physx::PxRigidActor *actor;

        explicit PhysicsActor(class Mesh *mesh);
    };
}


#endif //ENGINE_PHYSICSACTOR_H
