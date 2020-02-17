//
// Created by root on 11/1/20.
//

#include <string>
#include <utility>

#include "ComponentId.h"
#include "Index.h"
#include "ComponentBase.h"
#include "Engine.h"

unsigned int Component::next_id = 0xFEED0000;

std::ostream &Component::operator<<(std::ostream &out, const Component::ClassId &id) {
    return out << (int) id;
}

Component::ComponentId::ComponentId(const Component::ComponentId &other) {
    id = other.id;
}

bool Component::ComponentId::operator<(const Component::ComponentId &other) const {
    return this->id < other.id;
}

std::ostream &Component::operator<<(std::ostream &out, const Component::ComponentId &id) {
    if (Engine::identifier.count(id) <= 0) return out << id.id;
    else return out << id.id << "[" << Engine::identifier[id] << "]";
}

Component::ComponentId::ComponentId(bool) noexcept : id(Component::next_id++) {}

Component::ComponentId::ComponentId() : id(0xFFFFFFFFu) {}

/**
 * Returns the ClassId associated with this component in the Engine; returns ClassId::None if no data is linked with
 * this component.
 */
Component::ClassId Component::ComponentId::classId() const {
    auto meta = base();
    return meta ? meta->classId() : Component::ClassId::None;
}

void Component::ComponentId::attachExistingComponent(Component::ComponentId componentId) const {
    Component::Index::addComponent(*this, componentId);
}

void
Component::ComponentId::destroyComponent(Component::ComponentId componentId) const {
    Component::Index::removeComponent(*this, componentId);
}

std::set<Component::ComponentId> Component::ComponentId::childComponentsOfClass(Component::ClassId classId) const {
    return Index::componentsOf(*this, classId);
}

Component::ComponentInterface *
Component::ComponentId::base() const {
    return Index::entityData<Component::ComponentInterface>(*this);
}
void Component::ComponentId::destroyComponentsOfType(Component::ClassId classId) const {
    auto components = Index::componentsOf(*this, classId);
    for (const auto &component : components) { destroyComponent(component); }
}


