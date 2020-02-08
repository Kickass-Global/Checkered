//
// Created by Jackson Cougar Wiebe on 2/8/2020.
//

#ifndef ENGINE_PHYSICSHANDLER_HPP
#define ENGINE_PHYSICSHANDLER_HPP

#include <glm/glm.hpp>
#include "ComponentBase.h"

namespace Component {
    class PhysicsPacket : public Component::ComponentBase<Component::ClassId::PhysicsPacket> {

    public:
        glm::vec3 position;
        explicit PhysicsPacket(glm::vec3 pos) : position(pos) {}

    };
}

#endif //ENGINE_PHYSICSHANDLER_HPP
