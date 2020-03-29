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
  engine.load_scenario<Start>();

  while (running) {
    engine.update();
  }
}
