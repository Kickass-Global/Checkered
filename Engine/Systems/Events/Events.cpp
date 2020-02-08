//
// Created by root on 20/1/20.
//

#include "Events.h"

std::set<Component::ComponentId> Engine::EventSystem::registeredHandlers;

void Engine::EventSystem::update(Engine::deltaTime) {

    // todo: combine EventArgs between updates so that handlers don't get showered with events. #optimization

    for (Component::ComponentId handler : registeredHandlers) {

        std::set<Component::ComponentId> set = Component::Index::componentsOf(
                handler);
        for (Component::ComponentId eventArgs : set) {

            // I want to take all the data and pass it to the callback of the handler
            auto data = eventArgs.data<Component::EventArgs<void>>();
            handler.data<Component::EventHandler<void>>()->callback(*data);

        }
        // remove eventArgs from handler
        for (auto &&id : set) {
            handler.destroyComponent(id);
        }
    }
}

void Engine::EventSystem::registerHandler(const Component::ComponentId &handler) {
    registeredHandlers.emplace(handler);
}