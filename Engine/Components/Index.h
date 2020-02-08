//
// Created by root on 11/1/20.
//

#ifndef ENGINE_INDEX_H
#define ENGINE_INDEX_H

#include <map>
#include <set>
#include <memory>

namespace Component {
    struct ComponentId;
    enum class ClassId : unsigned int;
    class CopmonentInterface;
}

namespace Component {

    class Index {

    private:

        static std::map<Component::ComponentId, std::set<Component::ComponentId>> entityComponents;
        static std::map<Component::ClassId, std::set<Component::ComponentId>> entities;
        static std::map<Component::ComponentId, std::shared_ptr<void>> meta;

    public:

        template<typename T>
        static void push_entity(Component::ClassId cid, Component::ComponentId id, std::shared_ptr<T> &data) {

            meta[id] = data;
            entities[cid].emplace(id);
            entityComponents[id] = {};

        }

        template<typename T>
        static void replace_component_data(Component::ComponentId id,
                                           std::shared_ptr<T> &data) {

            data->clone(id);
            meta[id] = data;

        }

        static const std::set<Component::ComponentId> &
        entitiesOf(Component::ClassId cid);

        static std::set<Component::ComponentId> Component::Index::componentsOf
                (Component::ComponentId id, Component::ClassId classId);

        static const std::set<Component::ComponentId> &
        componentsOf(Component::ComponentId id);

        static bool hasComponent(Component::ComponentId id,
                                 Component::ComponentId componentId);

        static void
        addComponent(Component::ComponentId eid, Component::ComponentId cid);

        static void
        removeComponent(Component::ComponentId eid, Component::ComponentId cid);

        template<class T>
        static std::shared_ptr<T> entityData(const Component::ComponentId &componentId) {
            auto it = meta.find(componentId);

            if (it == meta.end()) return nullptr;
            
            return std::static_pointer_cast<T>(it->second);
        }
    };

}

#endif //ENGINE_INDEX_H
