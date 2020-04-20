#ifndef SCENARIOS_TESTWORLD_H
#define SCENARIOS_TESTWORLD_H

#include "Rendering/text.hpp"
#include "scenariointerface.hpp"
#include "timer.hpp"
#include <Billboard.h>
#include <Engine.h>
#include <Pipeline/Library.h>
#include <Vehicle.h>
#include <WorldTransform.h>
#include <optional>
#include <scenery.hpp>

namespace Component {}// namespace Component

class TestWorld : public Engine::ScenarioInterface {
  public:
  private:
  public:
  std::shared_ptr<Timer> timer;
  TestWorld(Engine::EngineSystem *enginePtr);

  private:
  void load() override;

  void update(deltaTime d) override {
    if (timer) timer->tick();

    auto fps =
        getEngine()->createMomentaryComponent<Component::Text>("Frametime: " + std::to_string(d));
    fps->font_size = 4;
    fps->plot = {10, 10, 100, 40};
    fps->dst = {-1, -1};
    fps->src = {-1, -1};
    fps->vertical_align = eVerticalAlign::bottom;
    fps->align = eAlign::left;
  }
};

#endif// SCENARIOS_TESTWORLD_H