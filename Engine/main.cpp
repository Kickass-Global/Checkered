// Engine.cpp : Defines the entry point for the application.
//

#include <functional>

#include "main.h"
#include "Systems/Pipeline/EntityLoader.h"
#include "Systems/Damage/damagesystem.hpp"
#include <PxPhysicsAPI.h>
#include <Vehicle/vehiclesystem.hpp>
#include <Vehicle.h>
#include "Passenger.h"
#include "ReportCard.h"

#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include "Systems/Component/scenecomponentsystem.hpp"
#include "Systems/Navigation/astar.h"
#include "Systems/systeminterface.hpp"
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
