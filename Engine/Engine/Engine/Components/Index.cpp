//
// Created by root on 11/1/20.
//

#include "Index.h"

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
