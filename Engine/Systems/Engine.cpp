//
// Created by root on 18/1/20.
//

#include <algorithm>
#include <sstream>
#include "Engine.h"
#include "EventHandler.h"
#include "Events/Events.h"

//const char Engine::module[] = "Engine";

std::vector<std::unique_ptr<Engine::SystemInterface>> Engine::registeredSystems;

void Engine::nameComponent(const Component::ComponentId &componentId,
                           std::string name) {
    identifier[componentId] = name;
}

const std::vector<std::unique_ptr<Engine::SystemInterface>> &Engine::systems() {
    return registeredSystems;
}

std::ostream &operator<<(std::ostream &out, Engine::Name name) {
    const unsigned int width = 20;

    std::ostringstream buff;
    buff << "[";
    std::copy_n(name.value.begin(),
                std::min(name.value.size(), static_cast<size_t>(width - 3)),
                std::ostreambuf_iterator(buff));
    buff << "] ";

    return out << std::setw(width) << buff.str();
}

std::map<Component::ComponentId, std::string> Engine::identifier;

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
