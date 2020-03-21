//
// Created by root on 18/1/20.
//

#include <algorithm>
#include <sstream>
#include "Engine.h"
#include "EventHandler.h"
#include <glm/glm.hpp>
#include <iomanip>
#include <Pipeline/Library.h>
#include <glm/gtc/quaternion.hpp>
#include <Events/Events.h>

EngineSystem *Engine::current = nullptr;

std::ostream &Engine::operator<<(std::ostream &out, const glm::vec3 &vector) {

    return out << "<" << vector.x << ", " << vector.y << ", " << vector.z << ">";
}

std::ostream &Engine::operator<<(std::ostream &out, const glm::quat &quat) {

    return out << "<" << quat.x << ", " << quat.y << ", " << quat.z << ", " << quat.w << ">";
}

std::ostream &Engine::operator<<(std::ostream &out, Engine::Name name) {

    const unsigned int width = 20;
    std::ostringstream buff;
    buff << "[";
    std::copy_n(name.value.begin(),
                std::min(name.value.size(), static_cast<size_t>(width - 3)),
                std::ostreambuf_iterator(buff));
    buff << "] ";

    return out << std::setw(width) << buff.str();
}

std::chrono::time_point time_of_previous_frame = std::chrono::high_resolution_clock::now();
bool paused = false;

void Engine::EngineSystem::pause() {
    log<high>("pause()");
    paused = true;
}

void Engine::EngineSystem::unpause() {
    log<high>("unpause()");
    paused = false;
    time_of_previous_frame = std::chrono::high_resolution_clock::now();
}

float tick() {

    if (paused) {
        return 0;
    }

    auto current_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = current_time - time_of_previous_frame;
    time_of_previous_frame = current_time;

    auto result =
        std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time).count() / 1000.0f;
    return result;
}

void Engine::EngineSystem::update() {
    update(tick());
}

void Engine::EngineSystem::update(deltaTime time) {
    scenario->update(time);
    SystemInterface::update(time);
}

void Engine::EngineSystem::initialize() {
    enginePtr = this;
    store = addSubSystem<EngineStore>();
    addSubSystem<EventSystem>();
    addSubSystem<Pipeline::Library>();
    addSubSystem<Pipeline::MeshLoader>();
    addSubSystem<Pipeline::ProgramLoader>();
    addSubSystem<Pipeline::ShaderLoader>();
    SystemInterface::initialize();
}
