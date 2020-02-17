//
// Created by root on 11/1/20.
//

#include <algorithm>
#include <vector>
#include <iterator>

#include "Index.h"
#include "ComponentId.h"
#include "ComponentBase.h"

using namespace Component;
using namespace Engine;


std::map<ComponentId, std::set<ComponentId>> Index::entityComponents;
std::map<std::type_index, std::set<ComponentId>> Index::entities;
std::map<ComponentId, std::unique_ptr<ComponentInterface>> Index::meta;
std::map<ComponentId, int> Index::ttl;
std::map<ComponentId, Reference> Index::refs;

std::set<ComponentId> Index::componentsOf
        (ComponentId id, ClassId classId) {
    std::set<ComponentId> children;
    // Create a copy of all children with matching classId.
    std::copy_if(entityComponents[id].begin(), entityComponents[id].end(),
                 std::inserter(children, children.end()),
                 [&classId](const ComponentId &
                 componentId) {
                     return
                             componentId.classId() == classId;
                 });

    return children;
}

void Index::addComponent(ComponentId eid, ComponentId cid) {
    entityComponents[eid].emplace(cid);
    refs[cid].count = refs[cid].count == std::numeric_limits<int>::max() ? 1 : refs[cid].count++;
}

void Index::removeComponent(ComponentId eid, ComponentId cid) {
    entityComponents[eid].erase(cid);
    refs[cid].count--;
}
void Index::set_ttl(ComponentId cid, int ttl) {
    Index::ttl[cid] = ttl;
}
void Index::update(Engine::deltaTime) {

    {
        std::vector<ComponentId> garbage;
        for (auto&[cid, life] : ttl) {
            life--;
            if (ttl.count(cid) > 0 && ttl[cid] <= 0) {
                entityComponents.erase(cid);
                garbage.push_back(cid);
            }
        }
        for (const auto &cid : garbage) ttl.erase(cid);
        garbage.clear();
    }

    {
        std::vector<std::tuple<ComponentId, Reference>> garbage;

        for (auto&[cid, reference] : refs) {
            if (reference.count <= 0) {
                garbage.emplace_back(cid, reference);
            }
        }

        for (const auto&[cid, reference] : garbage) {
            entities[reference.type].erase(cid);
            refs.erase(cid);
            entityComponents.erase(cid);
            meta.erase(cid);
        }
    }
}

void Index::initialize() {
    SystemInterface::initialize();
}
