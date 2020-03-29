//
// Created by Jackson Cougar Wiebe on 1/27/2020.
//

#include "damagesystem.hpp"
#include <EngineStore.h>
#include <WorldTransform.h>

void Engine::DamageSystem::update(Engine::deltaTime elapsed) {

  auto models = getEngine()
                    ->getSubSystem<EngineStore>()
                    ->getRoot()
                    .getComponentsOfType<Component::Model>();

  for (auto &model : models) {

    const bool is_dirty = model->is_outdated;

    if (is_dirty) {
      log<medium>("Updating dirty model#", model);
      model->is_outdated = false;
      for (auto &&part : model->parts) {

        auto &&mesh = part.variations[part.active_variation].mesh;
        if (mesh) {
          mesh->getStore().eraseComponentsOfType<Component::WorldTransform>();
          mesh->getStore().emplaceComponent<Component::WorldTransform>(
              model->transform);
        }
      }
    }

    // check to see if this model has received any damage...
    auto damages = model->getStore().getComponentsOfType<Damage>();

    if (!damages.empty()) {
      int total_damage = 0;
      for (auto &damage : damages) {
        total_damage += damage->damage_amount;
      }
      model->current_damage = std::clamp(model->current_damage + total_damage,
                                         0, model->max_damage);
    }
    model->getStore().eraseComponentsOfType<Damage>();

    for (auto &&part : model->parts) {

      auto it =
          std::find_if(part.variations.begin(), part.variations.end(),
                       [current_damage = model->current_damage](auto variation) {
                         return variation.damage_threshold > current_damage;
                       });

      if (it != part.variations.end()) {
        auto previous = part.active_variation;
        part.active_variation = static_cast<int>(it - part.variations.begin());

        auto &&mesh = part.variations[part.active_variation].mesh;


        auto variation_changed = part.active_variation != previous;

        if (variation_changed) {
          Engine::log("Updating variation#", part.active_variation);
        }
      }
    }
  }
}

void Engine::DamageSystem::initialize() {}
