//
// Created by root on 18/1/20.
//

#include "Engine.h"
#include "Components/EventHandler.h"
#include "Systems/Events/Events.h"

void Engine::nameComponent(const Component::ComponentId &componentId,
                           std::string name) {
    identifier[componentId] = std::move(name);
}

std::map<Component::ComponentId, std::string> Engine::identifier;