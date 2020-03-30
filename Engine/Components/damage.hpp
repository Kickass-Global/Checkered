//
// Created by Jackson Cougar Wiebe on 1/27/2020.
//

#pragma once

#ifndef ENGINE_DAMAGE_HPP
#define ENGINE_DAMAGE_HPP

#include "ComponentBase.h"
#include "Mesh.h"
#include <vector>

namespace Component {

/**
 * This component communicates damage between entities
 */
class Damage : public ComponentBase {
public:
  int damage_amount;
  Damage(int damageAmount);
};

/**
 * This component controls how entities change in response to damage and
 * other effects.
 */
class Model : public ComponentBase {
public:
  EventDelegate<int> onHealthChanged{"onHit"};
  struct Variation {
    int damage_threshold;
    std::shared_ptr<Component::PaintedMesh> mesh;
  };

  struct Part {
    std::vector<Variation> variations = {};
    int active_variation;
  };
  bool is_outdated = false;
  glm::mat4 transform;
  int current_damage = 0;
  int max_damage = 100;
  std::vector<Part> parts;
};
} // namespace Component

#endif // ENGINE_DAMAGE_HPP
