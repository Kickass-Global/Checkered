#include <functional>

#include "Engine.h"
#include "Systems/Pipeline/EntityLoader.h"
#include "main.h"
#include <start.hpp>

int main() {

  // region initialize engine systems
  using namespace Engine;

  auto running = true;
  EngineSystem engine;

  engine.initialize();
  engine.load_scenario<TestWorld>();

  while (!glfwWindowShouldClose(
      engine.getSubSystem<Rendering::RenderingSystem>()->getWindow())) {
    engine.update();
  }
}
