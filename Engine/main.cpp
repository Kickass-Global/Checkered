// Engine.cpp : Defines the entry point for the application.
//

#include <functional>

#include "main.h"
#include "Systems/Pipeline/EntityLoader.h"
#include "Engine.h"
#include <start.hpp>

int main() {

    // region initialize engine systems
    using namespace Engine;

    auto running = true;
    EngineSystem engine;

    engine.initialize();
    engine.load_scenario<TestWorld>();

    while (running) {
        engine.update();
    }
}
