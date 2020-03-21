#ifndef SCENARIOS_TESTWORLD_H
#define SCENARIOS_TESTWORLD_H

#include "scenariointerface.hpp"
#include <Engine.h>
#include "rendering/text.hpp"

class TestWorld : public Engine::ScenarioInterface {
public:
private:
public:
    TestWorld(Engine::EngineSystem *enginePtr);
private:
    void load() override; 
    void update(deltaTime d) override {
        auto fps = getEngine()->createMomentaryComponent<Component::Text>(
            "Frametime: " + std::to_string(d));
        fps->font_size = 4;
        fps->plot = { 0,0,640,480 };
        fps->vertical_align = eVerticalAlign::bottom;
        fps->align = eAlign::left;
    }
};

#endif //SCENARIOS_TESTWORLD_H