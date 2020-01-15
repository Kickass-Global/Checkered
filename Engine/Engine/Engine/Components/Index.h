//
// Created by root on 11/1/20.
//

#ifndef ENGINE_INDEX_H
#define ENGINE_INDEX_H

#include <map>
#include <set>
#include <memory>
#include "ComponentId.h"

namespace Component {

    class Index {
    public:

    private:

        inline static std::map<Component::ComponentId, std::set<Component::ComponentId>> entityComponents;
        inline static std::map<Component::ClassId, std::set<Component::ComponentId>> entities;
        inline static std::map<Component::ComponentId, std::shared_ptr<void>> meta;

    public:

        template <typename T>
        static void push_entity(Component::ClassId cid, Component::ComponentId id, std::shared_ptr<T>& data) {

            meta[id] = data;
            entities[cid].emplace(id);
            entityComponents[id] = {};

        }

        template <typename T>
        static void replace_component_data(Component::ComponentId id, std::shared_ptr<T>& data)
        {
            meta[id] = data;
        }

        static const std::set<Component::ComponentId>& entitiesOf(Component::ClassId cid) {
            return entities[cid];
        }

        static const std::set<Component::ComponentId>& componentsOf(Component::ComponentId id)
        {
            return entityComponents[id];
        }

        static bool hasComponent(Component::ComponentId id, Component::ComponentId componentId)
        {
            return entityComponents[id].count(componentId) > 0;
        }

        static void addComponent(Component::ComponentId eid, Component::ComponentId cid)
        {
            entityComponents[eid].emplace(cid);
        }

        static void removeComponent(Component::ComponentId eid, Component::ComponentId cid)
        {
            entityComponents[eid].erase(cid);
        }

        template<typename T>
        static std::shared_ptr<T> entityData(const Component::ComponentId &componentId) {
            return std::static_pointer_cast<T>(meta[componentId]);
        }
    };

}


#endif //ENGINE_INDEX_H
