//
// Created by root on 17/1/20.
//

#pragma once
#ifndef ENGINE_EVENTDELEGATE_H
#define ENGINE_EVENTDELEGATE_H

#include <iostream>
#include <functional>
#include <vector>
#include <tuple>
#include <memory>

#include "Engine.h"
#include "Component.h"
#include "EventHandler.h"

namespace Component {


	/**
	 * Contains data from an event that will be sent to subscribers of that event.
	 * @tparam Args the data types to pass to subscribers
	 */
	template<typename... Args>
	class EventArgs : public ComponentBase
	{
	public:
		std::tuple<Args...> values;
		explicit EventArgs(Args... args) : values(args...) {}

		template <std::size_t I>
		typename  std::tuple_element<I, std::tuple<Args...> >::type get() const {
			return std::get<I>(values);
		}
	};

	/**
	 * Component events allow components to expose events; components can then subscribe to those events.
	 * This allows components to pass information between one another directly.
	 */
	template<typename... Args>
	class EventDelegate : public ComponentBase {

		std::vector<std::shared_ptr<EventHandler<Args...>>> subscribers{};
		std::vector<std::shared_ptr<std::function<void(Args...)>>> direct_subscribers{};

	public:

		explicit EventDelegate(std::string name);

		/**
		 * Invokes the event args to all listeners
		 * @param args the event args data
		 */
        void operator()(Args ... args);

		/**
		 * Adds a new subscriber to this event
		 * @param subscriber the component to notify when this event occurs.
		 */
		void operator+=(std::shared_ptr<EventHandler<Args...>> subscriber);
		void operator+=(std::function<void(Args...)> subscriber);
	};

	/**
	 * Invokes the event, passing the arguments to all subscribers.
	 * @tparam Args The types of the arguments to pass to subscribers
	 * @param args The arguments to pass to subscribers
	 */
	template<typename... Args>
	void EventDelegate<Args...>::operator()(Args... args) {

		Engine::log<module, Engine::low>("ComponentEvent#", id, " called.");

		for (auto listener : subscribers) {
			listener->emplaceChildComponent<Component::EventArgs<Args...>>(args...);
		}
		for (auto listener : direct_subscribers) {
			(*listener)(args...);
		}
	}

	/**
	 * Adds a new subscriber to this event.
	 * @param subscriber the subscribing component.
	 */
	template<typename... Args>
	void EventDelegate<Args...>::operator+=(std::shared_ptr<EventHandler<Args...>>  subscriber) {

		Engine::log<module>("Adding subscriber#", subscriber, " to ComponentEvent#", id);

		subscribers.push_back(subscriber);
		Engine::EventSystem::registerHandler<Args...>(subscriber);

	}

	/**
 * Adds a new subscriber to this event.
 * @param subscriber the subscribing component.
 */
	template<typename... Args>
	void EventDelegate<Args...>::operator+=(std::function<void(Args...)>  subscriber) {
		direct_subscribers.push_back(std::make_shared<std::function<void(Args...)>>(subscriber));
	}

	/**
	 * Creates a new component event.
	 * @param name the name of the component event.
	 */
	template<typename... Args>
	EventDelegate<Args...>::EventDelegate(std::string name) {
		Engine::nameComponent(id, name);
	}
}

#endif //ENGINE_EVENTDELEGATE_H
