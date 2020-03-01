//
// Created by Jackson Cougar Wiebe on 2/8/2020.
//

#pragma once

#ifndef ENGINE_PHYSICSPACKET_HPP
#define ENGINE_PHYSICSPACKET_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "ComponentBase.h"

namespace Component {
    class PhysicsPacket : public Component::ComponentBase<Component::ClassId::PhysicsPacket> {

    public:
        glm::vec3 position;
        glm::quat rotation;
        explicit PhysicsPacket(glm::vec3 pos, glm::quat rotation) : position(pos), rotation(rotation) {}

    };
}

#endif //ENGINE_PHYSICSPACKET_HPP
