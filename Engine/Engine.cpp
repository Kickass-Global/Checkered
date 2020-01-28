//
// Created by root on 18/1/20.
//

#include "Engine.h"
#include "Components/EventHandler.h"
#include "Systems/Events/Events.h"

namespace {
    std::vector<std::shared_ptr<Engine::SystemInterface>> registeredSystems;
}

void Engine::nameComponent(const Component::ComponentId &componentId,
                           std::string name) {
    identifier[componentId] = std::move(name);
}

void Engine::addSystem(const std::shared_ptr<Engine::SystemInterface> &system) {
    registeredSystems.push_back(system);
    system->initialize();
}

const std::vector<std::shared_ptr<Engine::SystemInterface>> &Engine::systems() {
    return registeredSystems;
}

std::map<Component::ComponentId, std::string> Engine::identifier;