//
// Created by root on 20/1/20.
//

#include "Events.h"

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

void EventSystem::early_update(Engine::deltaTime time) {
    onTick(time);
    SystemInterface::early_update(time);
}
