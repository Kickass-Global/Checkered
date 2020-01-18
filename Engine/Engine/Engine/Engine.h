//
// Created by root on 18/1/20.
//

#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include <iostream>
#include <functional>
#include <vector>

#include "Components/Index.h"
#include "Components/ComponentId.h"


namespace Component {
    template<typename... Args>
    class ComponentEvent;

    template<typename... As>
    class EventArgs;

    struct ComponentId;
    enum class ClassId : unsigned int;
}

namespace Engine {

    typedef float frametime;
    constexpr char module[] = "Engine";

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

    /**
     * Monitors registered components for EventArg data and passes it on to the corresponding event handler.
     * Notes:
     * A component must be registered with this system to receive events through ComponentEvent events.
     */
    class EventSystem {

        static std::set<Component::ComponentId> registeredHandlers;

    public:

        static void update(frametime /*elapsed*/);

        static void registerHandler(const Component::ComponentId &handler);

        template<typename T, typename... Args>
        static Component::ComponentId
        createHandler(T *instance, void (T::*callback)(const Component::EventArgs<Args...> &)) {

            std::shared_ptr<Component::EventHandler<Args...>> handler = Engine::createComponent<Component::EventHandler<Args...>>();
            handler->callback = std::bind(callback, instance, std::placeholders::_1);
            return handler->id();

        }
    };

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

    template<typename T>
    std::shared_ptr<T> replaceComponent(Component::ComponentId id, std::shared_ptr<T> component) {
        Component::Index::replace_component_data(id, component);
        auto result = Component::Index::entityData<T>(id);
        return result;

    }

}

#endif //ENGINE_ENGINE_H
