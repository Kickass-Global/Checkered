//
// Created by root on 17/1/20.
//

#ifndef ENGINE_COMPONENTEVENT_H
#define ENGINE_COMPONENTEVENT_H

#include <iostream>
#include <functional>
#include <vector>
#include <tuple>

#include "ComponentId.h"
#include "ComponentBase.h"
#include "Index.h"
#include "EventHandler.h"

#include "../Engine.h"

namespace Component {

    struct ComponentId;

    template<typename... Args>
    class EventArgs : public ComponentBase<ClassId::EventArgs>
    {
    public:
        explicit EventArgs(Args... args) : values(args...) {}
        std::tuple<Args...> values;
    };

    template<typename... Args>
    class ComponentEvent : public ComponentBase<ClassId::Event>{

        std::vector<Component::ComponentId> listeners;

    public:

        void operator()(Args ... args);

        void operator+=(Component::ComponentId listener) {

            listeners.push_back(listener);
            Engine::EventSystem::registerHandler(listener);

        }
    };

    template<typename... Args>
    void ComponentEvent<Args...>::operator()(Args... args) {
        for (Component::ComponentId listener : listeners) {

            auto eventArgs = std::make_shared<EventArgs<Args...>>(args...);
            Index::push_entity(eventArgs->classId(), eventArgs->id(), eventArgs);
            listener.get()->attachComponent(eventArgs->id());

        }
    }
}


#endif //ENGINE_COMPONENTEVENT_H
