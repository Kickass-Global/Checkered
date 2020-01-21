//
// Created by root on 17/1/20.
//

#ifndef ENGINE_COMPONENTEVENT_H
#define ENGINE_COMPONENTEVENT_H

#include <iostream>
#include <functional>
#include <vector>
#include <tuple>

#include "Component.h"

#include "../Engine.h"
#include "../Systems/Events/Events.h"

namespace Component {

    struct ComponentId; //fwd decl.

    /**
     * Contains data from an event that will be sent to subscribers of that event.
     * @tparam Args the data types to pass to subscribers
     */
    template<typename... Args>
    class EventArgs : public ComponentBase<ClassId::EventArgs>
    {
    public:
        explicit EventArgs(Args... args) : values(args...) {}
        std::tuple<Args...> values;
    };

    /**
     * Component events allow components to expose events; components can then subscribe to those events.
     * This allows components to pass information between one another directly.
     */
    template<typename... Args>
    class ComponentEvent : public ComponentBase<ClassId::Event>{

        std::vector<Component::ComponentId> subscribers;

    public:

        ComponentEvent(std::string name);

        /**
         * Invokes the event args to all listeners
         * @param args the event args data
         */
        void operator()(Args ... args);

        /**
         * Adds a new subscriber to this event
         * @param subscriber the component to notify when this event occurs.
         */
        void operator+=(Component::ComponentId subscriber);
    };

    template<typename... Args>
    void ComponentEvent<Args...>::operator()(Args... args) {

        Engine::log<module>("ComponentEvent#", id(), " called.");

        for (Component::ComponentId listener : subscribers) {

            auto eventArgs = std::make_shared<EventArgs<Args...>>(args...);
            Index::push_entity(eventArgs->classId(), eventArgs->id(), eventArgs);
            listener.data()->attachComponent(eventArgs->id());

        }
    }

    template<typename... Args>
    void ComponentEvent<Args...>::operator+=(Component::ComponentId subscriber) {

        Engine::log<module>("Adding subscriber#", subscriber, " to ComponentEvent#", id());

        subscribers.push_back(subscriber);
        Engine::EventSystem::registerHandler(subscriber);

    }

    template<typename... Args>
    ComponentEvent<Args...>::ComponentEvent(std::string name) {
        Engine::nameComponent(id(), name);
    }
}

#endif //ENGINE_COMPONENTEVENT_H
