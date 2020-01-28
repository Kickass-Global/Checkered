//
// Created by root on 20/1/20.
//

#ifndef ENGINE_EVENTS_H
#define ENGINE_EVENTS_H

#include <functional>
#include <vector>

#include "../../Engine.h"
#include "../../Components/EventHandler.h"

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

/**
 * Monitors registered components for EventArg data and passes it on to the corresponding event handler.
 * Notes:
 * A component must be registered with this system to receive events through ComponentEvent events.
 */
class EventSystem : public Engine::SystemInterface {

    static std::set<Component::ComponentId> registeredHandlers;

public:

    void update(Engine::deltaTime /*elapsed*/) override;

    static void registerHandler(const Component::ComponentId &handler);

    template<typename T, typename... Args>
        static Component::ComponentId createHandler(
                T *instance, void (T::*callback)(const Component::EventArgs<Args...> &)) {

            std::shared_ptr<Component::EventHandler<Args...>> handler = Engine::createComponent<Component::EventHandler<Args...>>();
            handler->callback = std::bind(callback, instance, std::placeholders::_1);
            return handler->id();

        }
    };

}
#endif //ENGINE_EVENTS_H
