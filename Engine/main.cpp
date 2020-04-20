#include <functional>

#include "Engine.h"
#include "Systems/Pipeline/EntityLoader.h"
#include <start.hpp>

int main() {

  // region initialize engine systems
  using namespace Engine;

  // disable std::out 
  //std::cout.setstate(std::ios_base::failbit);


  auto running = true;
  EngineSystem engine;

  engine.initialize();
  engine.load_scenario<Start>();

  do {
    engine.update();
  } while (!glfwWindowShouldClose(engine.getSubSystem<Rendering::RenderingSystem>()->getWindow()));
}
