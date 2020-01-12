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

        enum Type {
            Camera,
            Shader
        };

    private:

        inline static std::map<Component::ComponentId, std::set<Component::ComponentId>> entityComponents;
        inline static std::map<Type, std::set<Component::ComponentId>> entities;
        inline static std::map<Component::ComponentId, std::shared_ptr<void>> meta;

    public:

        template <typename T>
        static void push_entity(Type type, Component::ComponentId id, std::shared_ptr<T>& data) {
            meta[id] = data;
            entities[type].emplace(id);
            entityComponents[id] = {};
        }

        static const std::set<Component::ComponentId>& entitiesOf(Type type) {
            return entities[type];
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
