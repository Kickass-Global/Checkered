//
// Created by Jackson Cougar Wiebe on 2/16/2020.
//

#pragma once

#ifndef ENGINE_WORLDTRANSFORM_H
#define ENGINE_WORLDTRANSFORM_H

#include "ComponentBase.h"
#include "ComponentId.h"
#include <foundation/PxTransform.h>
#include <glm/detail/type_mat4x4.hpp>
#include <glm/fwd.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Component {

class WorldTransform : public ComponentBase {
public:
  glm::vec3 scale;
  glm::quat rotation;
  glm::vec3 position;

  glm::mat4 world_matrix{1};

  WorldTransform() : world_matrix(1) {}

  explicit WorldTransform(const physx::PxTransform &T)
      : position(T.p.x, T.p.y, T.p.z), rotation(T.q.w, T.q.x, T.q.y, T.q.z) {
    world_matrix = glm::translate(world_matrix, {T.p.x, T.p.y, T.p.z}) *
                   glm::mat4_cast(glm::quat(T.q.w, T.q.x, T.q.y, T.q.z));
  }

  explicit WorldTransform(glm::mat4 T) : world_matrix(T) {

    glm::vec4 perspective;
    glm::vec3 skew;
    glm::decompose(T, scale, rotation, position, skew, perspective);
  }
};

} // namespace Component

#endif // ENGINE_WORLDTRANSFORM_H
