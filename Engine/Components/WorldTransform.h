//
// Created by Jackson Cougar Wiebe on 2/16/2020.
//

#pragma once

#ifndef ENGINE_WORLDTRANSFORM_H
#define ENGINE_WORLDTRANSFORM_H

#include <glm/fwd.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include "ComponentId.h"
#include "ComponentBase.h"

namespace Component {

    class WorldTransform : public Component::ComponentBase<Component::ClassId::Transform> {
    public:
        glm::mat4 world_matrix{1};
        WorldTransform() : world_matrix(1) {}
        explicit WorldTransform(glm::mat4 T) : world_matrix(T) {}
    };

}

#endif //ENGINE_WORLDTRANSFORM_H
