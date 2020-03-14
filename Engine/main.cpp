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
    Start s(&engine);
    s.load();
    //TestWorld::load();

    // region initialize game-clocks
    using namespace std::chrono;
    typedef duration<float> floatMilliseconds; // define this to get float values;
    auto start = high_resolution_clock::now();
    auto end = start + milliseconds(1); // do this so physx doesn't complain about time being 0.
    // endregion

    while (running) {

        // region before update
        floatMilliseconds delta = end - start;
        Engine::deltaTime elapsed = static_cast<Engine::deltaTime>(duration_cast<milliseconds>(delta).count());
        // endregion

        s.update(elapsed);

        engine.getSubSystem<EventSystem>()->onTick(elapsed);
        engine.update(elapsed);

        // region after update
        start = end;
        end = high_resolution_clock::now();
        //endregion
    }
}
