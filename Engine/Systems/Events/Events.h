//
// Created by root on 20/1/20.
//

#pragma once

#ifndef ENGINE_EVENTS_H
#define ENGINE_EVENTS_H

#include <functional>
#include <vector>

#include "Engine.h"
#include "../../Systems/systeminterface.hpp"
#include "../../Components/EventHandler.h"
#include <EventDelegate.h>

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

		static std::vector<std::shared_ptr<EventHandlerBase>> registeredHandlers;

	public:
		static Component::EventDelegate<Engine::deltaTime> onTick;

		void update(Engine::deltaTime /*elapsed*/) override;

		template<typename... Args>
		static void registerHandler(std::shared_ptr<EventHandler<Args...>> handler) {
			registeredHandlers.emplace_back(handler);
		}

		template<typename T, typename... Args>
		static std::shared_ptr<EventHandler<Args...>> createHandler(
			T *instance,
			void (T::*callback)(const Component::EventArgs<Args...> &)) {

			auto handler = Engine::createComponent<Component::EventHandler<Args...>>();
			handler->callback = std::bind(callback, instance,
				std::placeholders::_1
			);
			return handler;

		}

		template<typename T>
		static std::shared_ptr<EventHandler<Engine::deltaTime>> createTickHandler(
			T *instance,
			void (T::* callback)(const Component::EventArgs<Engine::deltaTime> &)) {

			auto handler = Engine::createComponent<Component::EventHandler<Engine::deltaTime>>();
			handler->callback = std::bind(callback, instance, std::placeholders::_1);

			onTick += handler;
			return handler;
		}


		template<typename... Args>
		static std::shared_ptr<EventHandler<Args...>> createHandler(
			std::function<void(const Component::EventArgs<Args...> &)> callback) {

			auto handler = Engine::createComponent<Component::EventHandler<Args...>>();
			handler->callback = callback;
			return handler;

		}
	};
}
#endif //ENGINE_EVENTS_H
