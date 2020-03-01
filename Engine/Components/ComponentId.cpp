//
// Created by root on 11/1/20.
//

#include <string>
#include <utility>

#include "ComponentId.h"
#include "Index.h"
#include "ComponentBase.h"
#include "Engine.h"

unsigned int Component::next_id = 80000000;

std::ostream &Component::operator<<(std::ostream &out, const Component::ClassId &id) {
    return out << (int) id;
}

Component::ComponentId::ComponentId(const Component::ComponentId &other)
    : id(other.id), type(other.type) {}

bool Component::ComponentId::operator<(const Component::ComponentId &other) const {
    return this->id < other.id;
}

std::ostream &Component::operator<<(std::ostream &out, const Component::ComponentId &id) {
    if (Engine::identifier.count(id) <= 0) return out << id.type << " " << id.id;
    else return out << id.id << "[" << Engine::identifier[id] << "]";
}


ComponentId &ComponentId::operator=(const ComponentId &other) {
    if (this == &other) return *this;
    Engine::assertLog(type == other.type || type == ClassId::None, "Checking ComponentId(ComponentId &other) type");
    this->id = other.id;
    type = other.type;
}


Component::ComponentId::ComponentId(ClassId type) noexcept
    : id(Component::next_id++), type(type) {}

Component::ClassId Component::ComponentId::classId() const { return type; }

Component::ComponentId::ComponentId() : id(0xFFFFFFFFu), type(ClassId::None) {}

void Component::ComponentId::attachExistingComponent(const ComponentId component) const {
    Component::Index::addComponent(*this, component);
}

void Component::ComponentId::destroyComponent(const Component::ComponentId& componentId) const {
	Component::Index::removeComponent(*this, componentId);
}

std::set<Component::ComponentId> Component::ComponentId::childComponentsOfClass(Component::ClassId classId) const {
	return Index::componentsOf(*this, classId);
}

void Component::ComponentId::attachTemporaryComponent(const ComponentId componentId, int ttl) const
{
	Component::Index::addComponent(*this, componentId, ttl);
}

void Component::ComponentId::destroyComponentsOfType(Component::ClassId classId) const {
    auto components = Index::componentsOf(*this, classId);
    for (const auto &component : components) { destroyComponent(component); }
}


