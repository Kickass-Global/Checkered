//
// Created by root on 29/2/20.
//

#include "SoundSystem.h"

void Engine::SoundSystem::initialize() {
    device = alcOpenDevice(nullptr);
    if (!device)
        Engine::assertLog(!device, "Loading alcDevice");
}

void Engine::SoundSystem::update(Engine::deltaTime) {}
