//
// Created by root on 11/1/20.
//

#include <algorithm>
#include <iterator>
#include "Index.h"
#include "ComponentId.h"

std::map<Component::ComponentId, std::set<Component::ComponentId>> Component::Index::entityComponents;
std::map<Component::ClassId, std::set<Component::ComponentId>> Component::Index::entities;
std::map<Component::ComponentId, std::shared_ptr<void>> Component::Index::meta;

const std::set<Component::ComponentId> &
Component::Index::entitiesOf(Component::ClassId cid) {
    return entities[cid];
}

std::set<Component::ComponentId> Component::Index::componentsOf
        (Component::ComponentId id, Component::ClassId classId) {
    std::set<Component::ComponentId> children;
    // Create a copy of all children with matching classId.
    std::copy_if(entityComponents[id].begin(), entityComponents[id].end(),
                 std::inserter(children, children.end()),
                 [&classId](const Component::ComponentId &
                 componentId) {
                     return
                             componentId.classId() == classId;
                 });

    return children;
}

const std::set<Component::ComponentId> &
Component::Index::componentsOf(Component::ComponentId id) {
    return entityComponents[id];
}

bool Component::Index::hasComponent(Component::ComponentId id,
                                    Component::ComponentId componentId) {
    return entityComponents[id].count(componentId) > 0;
}

void Component::Index::addComponent(Component::ComponentId eid,
                                    Component::ComponentId cid) {
    //Engine::log<module>("Attaching component#", cid, " to #", eid);
    entityComponents[eid].emplace(cid);
}

void Component::Index::removeComponent(Component::ComponentId eid, Component::ComponentId cid) {
    entityComponents[eid].erase(cid);
}
