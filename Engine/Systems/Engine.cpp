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
    std::copy_n(
        name.value.begin(), std::min(name.value.size(), static_cast<size_t>(width - 3)),
        std::ostreambuf_iterator(buff));
    buff << "] ";

    return out << std::setw(width) << buff.str();
}

void Engine::EngineSystem::update(deltaTime time) {

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
