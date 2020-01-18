//
// Created by root on 11/1/20.
//

#include "Index.h"
#include "ComponentId.h"

std::map<Component::ComponentId, std::set<Component::ComponentId>> Component::Index::entityComponents;
std::map<Component::ClassId, std::set<Component::ComponentId>> Component::Index::entities;
std::map<Component::ComponentId, std::shared_ptr<void>> Component::Index::meta;

const std::set<Component::ComponentId> &Component::Index::entitiesOf(Component::ClassId cid) {
    return entities[cid];
}

const std::set<Component::ComponentId> &Component::Index::componentsOf(Component::ComponentId id) {
    return entityComponents[id];
}

bool Component::Index::hasComponent(Component::ComponentId id, Component::ComponentId componentId) {
    return entityComponents[id].count(componentId) > 0;
}

void Component::Index::addComponent(Component::ComponentId eid, Component::ComponentId cid) {
    entityComponents[eid].emplace(cid);
}

void Component::Index::removeComponent(Component::ComponentId eid, Component::ComponentId cid) {
    entityComponents[eid].erase(cid);
}
