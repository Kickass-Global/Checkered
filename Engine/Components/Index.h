//
// Created by root on 11/1/20.
//

#ifndef ENGINE_INDEX_H
#define ENGINE_INDEX_H

#include <map>
#include <set>
#include <memory>
#include <any>

namespace Component {
    struct ComponentId;
    enum class ClassId : unsigned int;

    class ComponentInterface;
}

namespace Component {

    class Index {

    private:

        static std::map<Component::ComponentId, std::set<Component::ComponentId>> entityComponents;
        static std::map<Component::ClassId, std::set<Component::ComponentId>> entities;
        static std::map<Component::ComponentId, std::unique_ptr<Component::ComponentInterface>> meta;

    public:

        template<typename T>
        static void push_entity(Component::ClassId cid, Component::ComponentId id, std::unique_ptr<T> data) {
            static_assert(std::is_base_of<Component::ComponentInterface, T>::value);

            meta.emplace(id, std::move(data));
            entities[cid].emplace(id);
            entityComponents[id] = {};

        }

        template<typename T>
        static void replace_component_data(Component::ComponentId id,
                                           std::unique_ptr<T> &data) {

            data->clone(id);
            meta[id] = data;

        }

        static const std::set<Component::ComponentId> &
        entitiesOf(Component::ClassId cid);

        static std::set<Component::ComponentId> componentsOf
                (Component::ComponentId id, Component::ClassId classId);

        static const std::set<Component::ComponentId> &
        componentsOf(Component::ComponentId id);

        static bool hasComponent(Component::ComponentId id,
                                 Component::ComponentId componentId);

        static void
        addComponent(Component::ComponentId eid, Component::ComponentId cid);

        static Component::ComponentId parentOf(Component::ComponentId
                                               componentId);

        static void
        removeComponent(Component::ComponentId eid, Component::ComponentId cid);

        template<class T>
        static T *entityData(const Component::ComponentId &componentId) {
            auto it = meta.find(componentId);

            auto result = it == meta.end() ? nullptr : (T *) it->second.get();
            return result;
        }
    };

}

#endif //ENGINE_INDEX_H
