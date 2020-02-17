//
// Created by root on 11/1/20.
//

#ifndef ENGINE_INDEX_H
#define ENGINE_INDEX_H

#include <map>
#include <set>
#include <memory>
#include <any>
#include <typeindex>
#include <tuple>
#include <ComponentId.h>
#include "systeminterface.hpp"

namespace Component {
    struct ComponentId;
    enum class ClassId : unsigned int;

    class ComponentInterface;
}

namespace Component {

    struct Reference {
        int count = 0;
        std::type_index type = typeid(Component::ClassId);
    };

    class Index : public Engine::SystemInterface {
    private:

        static std::map<Component::ComponentId, std::set<Component::ComponentId>> entityComponents;
        static std::map<std::type_index, std::set<Component::ComponentId>> entities;
        static std::map<Component::ComponentId, std::unique_ptr<Component::ComponentInterface>> meta;

        static std::map<ComponentId, Reference> refs;
        static std::map<ComponentId, int> ttl;

    public:

        void update(Engine::deltaTime /*elapsed*/) override;
        void initialize() override;

        template<typename T>
        static void push_entity(Component::ClassId cid, Component::ComponentId id, std::unique_ptr<T> data) {
            static_assert(std::is_base_of<Component::ComponentInterface, T>::value);

            meta.emplace(id, std::move(data));
            entities[typeid(T)].emplace(id);
            entityComponents[id] = {};
            refs[id] = {std::numeric_limits<int>::max(), typeid(T)};
        }

        template<typename T>
        static inline typename std::enable_if<std::is_base_of<Component::ComponentInterface, T>::value, T>::type
        *allocate() {

            T *result = nullptr;
            for (auto cid : entities[typeid(T)]) {

                if (refs.count(cid) > 0 && refs[cid].count <= 0) {
                    result = static_cast<T *>(meta[cid].get());
                    break;
                }

            }

            return result;
        }

        static void set_ttl(Component::ComponentId, int ttl);

        template<typename T>
        static const std::set<Component::ComponentId> &
        entitiesOf() {
            return entities[typeid(T)];
        }

        static std::set<Component::ComponentId> componentsOf
                (Component::ComponentId id, Component::ClassId classId);


        template<typename T>
        static void addTag(Component::ComponentId eid) {
            static_assert(std::is_base_of<Component::ComponentTagInterface, T>::value);

            auto tagId = T().id();
            Engine::log<module, Engine::low>("Adding tag#", tagId, "to component#", eid);
            entityComponents[eid].emplace(tagId);
        }

        template<typename T>
        static bool hasTag(Component::ComponentId eid, bool removeTag = true) {
            static_assert(std::is_base_of<Component::ComponentTagInterface, T>::value);

            auto tagId = T().id();
            auto result = entityComponents[eid].count(tagId) > 0;

            if (removeTag) {
                Engine::log<module, Engine::low>("Removing tag#", tagId, "to component#", eid);
                entityComponents[eid].erase(tagId);
            }

            return result;
        }

        static void addComponent(Component::ComponentId eid,
                                 Component::ComponentId cid);

        static void removeComponent(Component::ComponentId eid,
                                    Component::ComponentId cid);

        template<class T>
        static T *entityData(const Component::ComponentId &componentId) {
            auto it = meta.find(componentId);

            auto result = it == meta.end() ? nullptr : (T *) it->second.get();
            return result;
        }
    };

}

#endif //ENGINE_INDEX_H
