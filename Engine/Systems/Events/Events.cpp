//
// Created by root on 20/1/20.
//

#include "Events.h"

std::vector<std::shared_ptr<EventHandlerBase>> Engine::EventSystem::registeredHandlers;

Component::EventDelegate<Engine::deltaTime> Engine::EventSystem::onTick("onTick");

void Engine::EventSystem::update(Engine::deltaTime time) {

	// todo: combine EventArgs between updates so that handlers don't get showered with events. #optimization

	bool events_pending;
	do {
		events_pending = false;
		for (auto& handler : registeredHandlers) {
			events_pending |= handler->invoke_all();
		}
	} while (events_pending);
}
