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
#include "SystemInterface.hpp"
#include "ComponentInterface.h"

using Component::ComponentInterface;
using Component::Node;

// Index 2.0
// This is where data gets stored in the engine long-term
class EngineStore : public Engine::SystemInterface {

public:

    template<typename T>
    struct ComponentInstance {
        friend struct std::hash<ComponentInstance>;

        mutable unsigned int is_dirty : 1;
        std::shared_ptr<T> data;;

        ComponentInstance(unsigned int isDirty, std::shared_ptr<ComponentInterface> data) : is_dirty(isDirty), data(
            std::dynamic_pointer_cast<T>(
                data
            )) {}


        bool operator==(const ComponentInstance &other) const {

            return !data.owner_before(other.data) && !other.data.owner_before(data);
        }

        bool operator<(const ComponentInstance &other) const {

            return data.owner_before(other.data);
        }
    };


private:

    Node root; // this is the root of the component tree (owner)

    std::unordered_map<std::type_index, std::vector<std::weak_ptr<ComponentInterface>>> component_types; // one to many

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

        static_assert(std::is_base_of<ComponentInterface, T>::value);

        // check to see if the store has an available allocation we can reuse
        int uses = 0;
        auto it = std::find_if(  // O(n) #todo
            component_types[typeid(T)].begin(), component_types[typeid(T)].end(),
            [&uses](std::weak_ptr<ComponentInterface> ptr) {
                return ptr.expired();
            }
        );

        if (it != component_types[typeid(T)].end()) {
            auto shared = it->lock();

            shared.reset(new T(args...)); // copy-construct a new component using existing allocation...

            return std::static_pointer_cast<T>(shared);
        }
        // else create and add a new one...
        return addComponent(std::make_unique<T>(args...));
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
