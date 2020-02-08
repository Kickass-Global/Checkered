//
// Created by root on 11/1/20.
//

#ifndef ENGINE_DIRTY_H
#define ENGINE_DIRTY_H

#include <glm/fwd.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include "ComponentId.h"
#include "ComponentBase.h"

namespace Component {

class WorldTransform : public Component::ComponentBase<Component::ClassId::Transform> {
public:
    glm::mat4 world_matrix{1};
};

class Dirty {
public:
    static ComponentId id() { return {true, 0xAAAA0001}; }
};

class Visible {
public:
    static ComponentId id() { return {true, 0xAAAA0002}; }
};

}

#endif //ENGINE_DIRTY_H
