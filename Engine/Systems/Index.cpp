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


std::unordered_map<ComponentId, ClassMap> Index::child_components(8000);
ClassMap Index::scene(8000);
std::unordered_map<ComponentId, std::unique_ptr<ComponentInterface>> Index::meta(8000);
std::unordered_map<ComponentId, TTL> Index::ttl(8000);
std::unordered_map <ComponentId, Reference> Index::reference_count(8000);

std::set<ComponentId> Index::componentsOf
(ComponentId id, ClassId classId) {
	return child_components[id][classId];
}

void Index::update(Engine::deltaTime) {

	std::vector<ComponentId> garbage;
	for (auto&[cid, life] : ttl) {
		life.ttl--;
		if (ttl[cid].ttl < 0) {
			garbage.emplace_back(cid);
			//child_components[life.owner][typeid(T)].erase(cid);
			reference_count[cid].count = 0;
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
