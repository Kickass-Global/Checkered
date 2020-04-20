//
// Created by Jackson Cougar Wiebe on 4/19/2020.
//

#ifndef ENGINE_TIMER_HPP
#define ENGINE_TIMER_HPP
#include "Rendering/text.hpp"
#include "scenariointerface.hpp"
#include <Billboard.h>
#include <Engine.h>
#include <Pipeline/Library.h>
#include <Vehicle.h>
#include <WorldTransform.h>
#include <optional>
#include <scenery.hpp>

struct Timer : public ComponentBase {

  std::shared_ptr<Billboard> background;
  std::shared_ptr<Text> text;
  BoxModel bounds;

  Timer() {

    text = getEngine()->createComponent<Text>("");
    text->color = {0, 0, 1};
    text->font_size = 10;
    text->vertical_align = eVerticalAlign::middle;
    text->align = eAlign::center;
    text->plot = {0, 0, 200, 40};
    text->src = {0, 0};
    text->dst = {0, -0.7f};

    background = getEngine()->createComponent<Billboard>(
        getEngine()->createComponent<Texture>("Assets/Textures/Props.png"));
    background->plot = BoxModel{0, 0, 200, 40};
    background->src = {0, 0};
    background->dst = {0, -0.7f};
    getEngine()->getSubSystem<EngineStore>()->getRoot().deactivate(text.get());
    getEngine()->getSubSystem<EngineStore>()->getRoot().deactivate(background.get());
  }

  floatMilliseconds deadline = std::chrono::minutes(2);
  std::chrono::time_point<std::chrono::steady_clock> previous;
  std::chrono::time_point<std::chrono::steady_clock> now;

  void start(std::chrono::seconds time) {
    getEngine()->getSubSystem<EngineStore>()->getRoot().activate(text.get());
    getEngine()->getSubSystem<EngineStore>()->getRoot().activate(background.get());
    deadline = time;
    now = std::chrono::steady_clock::now();
  }

  void stop() {
    getEngine()->getSubSystem<EngineStore>()->getRoot().deactivate(text.get());
    getEngine()->getSubSystem<EngineStore>()->getRoot().deactivate(background.get()); }


  void tick() {

    previous = now;
    now = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - previous);
    deadline -= elapsed_time;

    text->set_text(deadline.count());

    if (deadline < std::chrono::seconds(0)) {
      // do something
    }
  }

  floatMilliseconds get_deadline() { return deadline; }
};
#endif//ENGINE_TIMER_HPP
