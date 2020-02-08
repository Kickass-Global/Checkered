//
// Created by root on 18/1/20.
//

#include "Engine.h"
#include "Components/EventHandler.h"
#include "Systems/Events/Events.h"

const char Engine::module[] = "Engine";

std::vector<std::unique_ptr<Engine::SystemInterface>> Engine::registeredSystems;

void Engine::nameComponent(const Component::ComponentId &componentId,
                           std::string name) {
    identifier[componentId] = std::move(name);
}

const std::vector<std::unique_ptr<Engine::SystemInterface>> &Engine::systems() {
    return registeredSystems;
}

std::map<Component::ComponentId, std::string> Engine::identifier;