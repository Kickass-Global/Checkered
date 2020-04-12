//
// Created by Jackson Cougar Wiebe on 4/3/2020.
//

#ifndef ENGINE_HEALTHBAR_HPP
#define ENGINE_HEALTHBAR_HPP

#include "Rendering/text.hpp"
#include "scenariointerface.hpp"
#include <Billboard.h>
#include <Engine.h>
#include <Pipeline/Library.h>
#include <Vehicle.h>
#include <WorldTransform.h>
#include <optional>
#include <scenery.hpp>

namespace Component {
struct HealthBar : public ComponentBase {
  std::shared_ptr<Model> target; // display the health of this
  std::shared_ptr<Texture> health_sprite;
  std::shared_ptr<Billboard> billboard;
  Engine::Rectangle health_sprite_dimensions;
  int previous_health;

  HealthBar() {

  }

  //updates health bar
  void updateHealthBar() {
      
      std::cout << "Current health: " << target->health << std::endl;
      getEngine()->getSubSystem<EngineStore>()->getRoot().deactivate(billboard.get());

      if (target->health == 100) {
          health_sprite =
              getEngine()->createComponent<Texture>("Assets/Textures/health_bar_100.png");
          health_sprite_dimensions = { 0, 0, 256, 64 };
          billboard = getEngine()->createComponent<Billboard>(health_sprite);
      }


      if (target->health > 75 && target->health < 100 ) {
          
          health_sprite =
              getEngine()->createComponent<Texture>("Assets/Textures/health_bar_75.png");
          health_sprite_dimensions = { 0, 0, 256, 64 };
          billboard = getEngine()->createComponent<Billboard>(health_sprite);
      }

      else if (target->health > 50 && target->health < 75) {
          getEngine()->getSubSystem<EngineStore>()->getRoot().deactivate(billboard.get());
          health_sprite =
              getEngine()->createComponent<Texture>("Assets/Textures/health_bar_50.png");
          health_sprite_dimensions = { 0, 0, 256, 64 };
          billboard = getEngine()->createComponent<Billboard>(health_sprite);
      }

      else if (target->health < 50) {
          getEngine()->getSubSystem<EngineStore>()->getRoot().deactivate(billboard.get());
          health_sprite =
              getEngine()->createComponent<Texture>("Assets/Textures/health_bar_25.png");
          health_sprite_dimensions = { 0, 0, 256, 64 };
          billboard = getEngine()->createComponent<Billboard>(health_sprite);
      }


  }

  //checks if health of car has changed since last frame
  bool healthHasChanged() {
      
      if (target->health != previous_health) {
          return true;
      }
      return false;
  }

  void tick(float time) {
    auto health_percentage = target->health  / target->max_health * 100.0f;

    if (healthHasChanged())
        updateHealthBar();

    
    health_sprite_dimensions.width = health_percentage * 3;

    auto [x, y, w, h] = health_sprite_dimensions;
    billboard->plot = BoxModel(w / 2, h / 2, w, h);
    billboard->dst = RelativeAnchor{-1, 1};
    billboard->src = {-1, 1};
    
    previous_health = target->health;
  }
};
} // namespace Component
#endif // ENGINE_HEALTHBAR_HPP
