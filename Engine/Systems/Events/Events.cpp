//
// Created by root on 20/1/20.
//

#include "Events.h"

std::set<Component::ComponentId> Engine::EventSystem::registeredHandlers;

Component::EventDelegate<Engine::deltaTime> Engine::EventSystem::onTick("onTick");

void Engine::EventSystem::update(Engine::deltaTime time) {

	// todo: combine EventArgs between updates so that handlers don't get showered with events. #optimization

	bool events_pending;
	do {
		events_pending = false;
		for (const Component::ComponentId& handler : registeredHandlers) {

			std::set<Component::ComponentId> set = handler.childComponentsOfClass(Component::ClassId::EventArgs);
			events_pending |= !set.empty(); // if any handlers have event args we want to keep proccessing...

			bool has_events_args = !set.empty();
			auto handler_data = handler.data<Component::EventHandler<>>();

			for (const Component::ComponentId& eventArgs : set) {

				// I want to take all the data and pass it to the callback of the handler
				auto data = eventArgs.data<Component::EventArgs<>>();
				handler_data->callback(*data);

			}

			handler.destroyComponentsOfType(Component::ClassId::EventArgs);
		}
	} while (events_pending);
}

void Engine::EventSystem::registerHandler(const Component::ComponentId &handler) {
	registeredHandlers.emplace(handler);
}