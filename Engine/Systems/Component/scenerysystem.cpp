//
// Created by Jackson Cougar Wiebe on 3/1/2020.
//

#include "scenerysystem.h"
#include <scenery.hpp>

void Engine::ScenerySystem::initialize() {
    SystemInterface::initialize();
}

void Engine::ScenerySystem::update(Engine::deltaTime) {
    for (const auto &scenery : Engine::getStore().getRoot().getComponentsOfType<Scenery>()) {
		
    }
}
