//
// Created by Jackson Cougar Wiebe on 3/13/2020.
//

#ifndef ENGINE_SCENARIOINTERFACE_HPP
#define ENGINE_SCENARIOINTERFACE_HPP

#include <Engine.h>

namespace Engine {
    class ScenarioInterface {
    protected:
        EngineSystem *enginePtr;
    public:

        explicit ScenarioInterface(EngineSystem *enginePtr) : enginePtr(enginePtr) {}

        EngineSystem *getEngine() { return enginePtr; }

        virtual void load() = 0;
    };
}


#endif //ENGINE_SCENARIOINTERFACE_HPP
