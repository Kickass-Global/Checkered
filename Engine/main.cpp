// Engine.cpp : Defines the entry point for the application.
//

#include "main.h"
#include "Engine.h"
#include <start.hpp>
#include "scenery.hpp"

int main() {

    // region initialize engine systems
    using namespace Engine;

    auto running = true;
    EngineSystem engine;

    engine.initialize();
    engine.load_scenario<Start>();

    while (running) {
        engine.update();
    }
}
