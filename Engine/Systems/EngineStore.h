//
// Created by Jackson Cougar Wiebe on 3/1/2020.
//

#ifndef ENGINE_ENGINESTORE_H
#define ENGINE_ENGINESTORE_H

#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <ComponentBase.h>
#include <memory>
#include <bitset>

using Component::ComponentInterface;


// Index 2.0
class EngineStore {

public:

    struct ComponentInstance {
        friend struct std::hash<ComponentInstance>;

        mutable unsigned int is_dirty : 1;
        ComponentInterface *data = nullptr;

        ComponentInstance(unsigned int isDirty, ComponentInterface *data);

        bool operator==(const ComponentInstance &other) const {
            return data == other.data;
        }
    };


private:

    typedef std::unordered_map<std::type_index, std::unordered_set<ComponentInstance>> Node;

    Node root; // this is the root of the component tree
    std::unordered_map<std::type_index, std::vector<std::shared_ptr<ComponentInterface>>> components;

public:
    template<typename T>
    std::set<T *> getComponentsOfType() const {
        static_assert(std::is_base_of<Component::ComponentInterface, T>::value);
        return root[typeid(T)];
    }

    template<typename T, typename... Args>
    std::shared_ptr<T> create(Args &... args) {
        static_assert(std::is_base_of<Component::ComponentInterface, T>::value);

        // check to see if the store has an available allocation we can reuse
        auto it = std::find_if(components[typeid(T)].begin(), components[typeid(T)].end(),
                               [](std::shared_ptr<ComponentInterface> ptr) {
                                   return ptr.use_count() <= 1;
                               }
        );
        if (it != components[typeid(T)].end()) {
            **it = T(args...); // copy-construct a new component using existing allocation...
            return std::static_pointer_cast<T>(*it);
        }
        // else create and add a new one...
        return addComponent<T>(std::make_unique<T>(args...));
    }


private:

    template<typename T>
    std::shared_ptr<T> addComponent(std::unique_ptr<ComponentInterface> component) {
        root[typeid(T)].emplace(true, component.get());
        auto it = components[typeid(T)].emplace_back(std::move(component));
        return std::static_pointer_cast<T>(it);;
    }

};

namespace std {
    template<>
    struct hash<EngineStore::ComponentInstance> {
        std::size_t operator()(EngineStore::ComponentInstance const &s) const noexcept {
            return reinterpret_cast<size_t>(&s.data);
        }
    };
}

#endif //ENGINE_ENGINESTORE_H
