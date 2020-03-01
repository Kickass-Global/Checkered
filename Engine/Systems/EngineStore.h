//
// Created by Jackson Cougar Wiebe on 3/1/2020.
//

#ifndef ENGINE_ENGINESTORE_H
#define ENGINE_ENGINESTORE_H


#include <unordered_map>
#include <typeindex>
#include <ComponentBase.h>
#include <memory>

using Component::ComponentInterface;


// Index 2.0
class EngineStore {

    struct ComponentInstance {
        unsigned int reserved = 0xFFFFFFFF; // future thoughts...
        ComponentInterface *data = nullptr;
    };

    typedef std::unordered_map<std::type_index, std::set<ComponentInstance>> Node;

    Node root; // this is the root of the component tree
    std::vector<std::shared_ptr<ComponentInterface>> components;

    template<typename T>
    std::set<T *> getComponentsOfType() const {
        static_assert(std::is_base_of<Component::ComponentInterface, T>::value);
        return root[typeid(T)];
    }

    template<typename T>
    void addComponent(std::unique_ptr<ComponentInterface> component) {
        root.emplace_back(typeid(T), {0, component.get()});
        components.emplace_back(std::move(component));
    }
};


#endif //ENGINE_ENGINESTORE_H
