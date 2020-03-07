//
// Created by root on 17/1/20.
//

#pragma once

#ifndef ENGINE_EVENTHANDLER_H
#define ENGINE_EVENTHANDLER_H

#include <functional>
#include "ComponentBase.h"

namespace Component {

	template<typename... Args>
	class EventArgs;


	class EventHandlerBase : public ComponentBase {
	public:
		virtual bool invoke_all() = 0;
	};

	template<typename... Args>
	class EventHandler : public EventHandlerBase {
	public:
		std::function<void(const Component::EventArgs<Args...>&)> callback;
		bool invoke_all() override {

			bool invoked_callback = false;
			auto args = getChildren().getComponentsOfType<EventArgs<Args...>>();
			for (auto& arg : args) {
				callback(*arg);
				invoked_callback = true;
			}
			getChildren().eraseComponentsOfType<Component::EventArgs<Args...>>();

			return invoked_callback;
		}
	};

}
#endif //ENGINE_EVENTHANDLER_H
