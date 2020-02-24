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


std::unordered_map<ComponentId, std::set<ComponentId>> Index::entityComponents(8000);
std::unordered_map<std::type_index, std::set<ComponentId>> Index::entities(8000);
std::unordered_map<ComponentId, std::unique_ptr<ComponentInterface>> Index::meta(8000);
std::unordered_map<ComponentId, TTL> Index::ttl(8000);
std::unordered_map <ComponentId, Reference> Index::refs(8000);

std::set<ComponentId> Index::componentsOf
(ComponentId id, ClassId classId) {
	std::set<ComponentId> children;
	// Create a copy of all children with matching classId.
	std::copy_if(entityComponents[id].begin(), entityComponents[id].end(),
		std::inserter(children, children.end()),
		[&classId](const ComponentId&
			componentId) {
				return
					componentId.classId() == classId;
		});

	return children;
}

void Index::addComponent(ComponentId eid, ComponentId cid, int ttl = -1) {
	entityComponents[eid].emplace(cid);
	if (ttl > 0) Index::ttl[cid] = { ttl, eid };
	refs[cid].count = refs[cid].count == std::numeric_limits<int>::max() ? 1 : refs[cid].count++;
}

void Index::removeComponent(ComponentId eid, ComponentId cid) {
	entityComponents[eid].erase(cid);
	refs[cid].count--;
}


void Index::update(Engine::deltaTime) {

	std::vector<ComponentId> garbage;
	for (auto& [cid, life] : ttl) {
		life.ttl--;
		if (ttl[cid].ttl <= 0) {
			garbage.emplace_back(cid);
			entityComponents[life.owner].erase(cid);
			refs[cid].count = 0;
			life.ttl = 0;
		}
	}
	for (auto cid : garbage) {
		//ttl.erase(cid);
	}

	// todo: use fixed allocation buffers...
	//{
	//    std::vector<std::tuple<ComponentId, Reference>> garbage;

	//    for (auto&[cid, reference] : refs) {
	//        if (reference.count <= 0) {
	//            garbage.emplace_back(cid, reference);
	//        }
	//    }

	//    for (const auto&[cid, reference] : garbage) {
	//        //entities[reference.type].erase(cid);
	//        //refs.erase(cid);
	//        //entityComponents.erase(cid);
	//        //meta.erase(cid);
	//    }
	//}
}

void Index::initialize() {
	SystemInterface::initialize();
}
