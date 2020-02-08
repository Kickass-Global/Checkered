//
// Created by root on 18/1/20.
//

#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include <map>
#include <memory>
#include <iostream>
#include <functional>
#include <utility>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"

#include "Components/ComponentId.h"
#include "Components/Index.h"
#include "Systems/systeminterface.hpp"


namespace Component {
    template<typename... Args>
    class ComponentEvent;

    template<typename... As>
    class EventArgs;

    struct ComponentId;
    enum class ClassId : unsigned int;
}

namespace Engine {

    extern std::vector<std::unique_ptr<Engine::SystemInterface>> registeredSystems;

    typedef float deltaTime;

    extern const char module[];

    const std::vector<std::unique_ptr<Engine::SystemInterface>> &systems();

    template<typename T>
    T &addSystem() {
        registeredSystems.push_back(std::make_unique<T>());
        registeredSystems.back()->initialize();
        return dynamic_cast<T &>(*registeredSystems.back());
    }

    extern std::map<Component::ComponentId, std::string> identifier;

    void nameComponent(
            const Component::ComponentId &componentId, std::string name);

    template<typename T>
    T *addComponent(std::unique_ptr<T> data) {
        static_assert(std::is_base_of<Component::ComponentInterface, T>::value);

        auto id = data->id();
        Component::Index::push_entity(data->classId(), data->id(), std::move(data));

        return id.data<T>();
    }

    template<typename T, typename... Args>
    T *createComponent(Args... args) {
        static_assert(std::is_base_of<Component::ComponentInterface, T>::value);

        auto component = std::make_unique<T>(args...);
        auto id = component->id();
        Component::Index::push_entity(component->classId(), component->id(), std::move(component));

        return id.data<T>();
    }

    template<typename T>
    std::shared_ptr<T> replaceComponent(Component::ComponentId id, std::shared_ptr<T> component) {
        Component::Index::replace_component_data(id, component);
        auto result = Component::Index::entityData<T>(id);
        return result;
    }
}

#endif //ENGINE_ENGINE_H
