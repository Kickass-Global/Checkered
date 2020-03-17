//
// Created by Jackson Cougar Wiebe on 3/1/2020.
//

#ifndef ENGINE_ENGINESTORE_H
#define ENGINE_ENGINESTORE_H

#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <memory>
#include <bitset>
#include <Node.hpp>
#include "SystemInterface.hpp"
#include "ComponentInterface.h"

using Component::ComponentInterface;
using Component::Node;

// Index 2.0
// This is where data gets stored in the engine long-term
class EngineStore : public Engine::SystemInterface {

private:

    Node root; // this is the root of the component tree (owner)

public:

    // The root holds components that are long-lived and must be removed manually.
    Node &getRoot() {
        return root;
    }

    /**
     * Constructs an object T with the supplied arguments in the store.
     * @tparam T the type of the object
     * @tparam Args the argument types
     * @param args the arguments to construct the object using
     * @return returns a shared_ptr to the constructed object.
     */
    template<typename T, typename... Args>
    std::shared_ptr<T> create(Args &... args) {
        return root.create<T>(args...);
    }

    void late_update(Engine::deltaTime time) override {
        root.prune();
    }

private:

    template<typename T>
    std::shared_ptr<T> addComponent(std::unique_ptr<T> component) {

        static_assert(std::is_base_of<ComponentInterface, T>::value);
        component->enginePtr = getEngine();
        std::shared_ptr<T> shared = std::move(component);
        component_types[typeid(T)].emplace_back(shared);

        return shared;
    }
};


#endif //ENGINE_ENGINESTORE_H
