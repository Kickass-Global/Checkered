#ifndef SCENARIOS_TESTWORLD_H
#define SCENARIOS_TESTWORLD_H

#include "scenariointerface.hpp"
#include <Engine.h>

class TestWorld : public Engine::ScenarioInterface {
public:
private:
public:
    TestWorld(Engine::EngineSystem *enginePtr);
private:
    void load() override;
};

#endif //SCENARIOS_TESTWORLD_H