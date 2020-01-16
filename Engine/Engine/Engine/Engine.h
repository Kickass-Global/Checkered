// Engine.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <functional>
#include "Components/Index.h"

namespace Engine {

    template<typename... Args>
    class Event {
        std::vector<std::function<void(Args...)>> handlers;
    public:

        void operator()(Args ... args) {
            for (auto &&handler : handlers) {
                if (handler) handler(std::forward<Args>(args)...);
            }
        }

        void operator+=(std::function<void(Args...)> handler) {
            handlers.push_back(handler);
        }
    };

    typedef float frametime;

    constexpr char module[] = "Engine";

    template<typename T>
    std::shared_ptr<T> createComponent() {
        auto component = std::make_shared<T>();
        Component::Index::push_entity(component->classId(), component->id(), component);

        return component;
    }

    template<typename T>
    std::shared_ptr<T> createComponent(std::shared_ptr<T> component) {
        Component::Index::push_entity(component->classId(), component->id(), component);
        return component;
    }
    
    template <typename T>
    std::shared_ptr<T> replaceComponent(Component::ComponentId id, std::shared_ptr<T> component) {
        Component::Index::replace_component_data(id, component);
        auto result =  Component::Index::entityData<T>(id);
        return result;

    }


}
// TODO: Reference additional headers your program requires here.
