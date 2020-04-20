//
// Created by root on 20/1/20.
//

#pragma once

#ifndef ENGINE_EVENTS_H
#define ENGINE_EVENTS_H

#include <functional>
#include <vector>
#include <memory>

#include "Engine.h"
#include "SystemInterface.hpp"
#include <EventHandler.h>
#include <EventDelegate.h>

namespace Engine {

    /**
     * Monitors registered components for EventArg data and passes it on to the corresponding event handler.
     * Notes:
     * A component must be registered with this system to receive events through ComponentEvent events.
     */
    class EventSystem : public Engine::SystemInterface {

        std::vector<std::shared_ptr<Component::EventHandlerBase>> registeredHandlers;

    public:
        Component::EventDelegate<Engine::deltaTime> onTick{"onTick"};

        void update(Engine::deltaTime /*elapsed*/) override;
        void early_update(Engine::deltaTime time) override;

        void reset() {
          registeredHandlers.clear();
        }

        template<typename... Args>
        void registerHandler(std::shared_ptr<Component::EventHandler<Args...>> handler) {

            registeredHandlers.emplace_back(handler);
        }

        template<typename T, typename... Args>
        std::shared_ptr<Component::EventHandler<Args...>> createHandler(
            T *instance, void (T::*callback)(const Component::EventArgs<Args...> &)) {

            auto handler = getEngine()->createComponent<Component::EventHandler<Args...>>();
            handler->callback = std::bind(
                callback, instance, std::placeholders::_1
            );
            return handler;

        }

        template<typename T>
        std::shared_ptr<Component::EventHandler<Engine::deltaTime>> createTickHandler(
            T *instance, void (T::* callback)(const Component::EventArgs<Engine::deltaTime> &)) {

            auto handler = getEngine()->createComponent<Component::EventHandler<Engine::deltaTime>>();
            handler->callback = std::bind(callback, instance, std::placeholders::_1);

            onTick += handler;
            return handler;
        }


        template<typename... Args>
        std::shared_ptr<Component::EventHandler<Args...>> createHandler(
            std::function<void(const Component::EventArgs<Args...> &)> callback
        ) {

            auto handler = getEngine()->createComponent<Component::EventHandler<Args...>>();
            handler->callback = callback;
            return handler;

        }
    };
}
#endif //ENGINE_EVENTS_H
