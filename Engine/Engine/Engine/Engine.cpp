//
// Created by root on 18/1/20.
//

#include "Engine.h"
#include "Components/EventHandler.h"

std::set<Component::ComponentId> Engine::EventSystem::registeredHandlers;

void Engine::EventSystem::update(Engine::frametime) {

    // todo: combine EventArgs between updates so that handlers don't get showered with events. #optimization

    for (Component::ComponentId handler : registeredHandlers) {

        std::set<Component::ComponentId> set = Component::Index::componentsOf(handler);
        for (Component::ComponentId eventArgs : set) {

            // I want to take all the data and pass it to the callback of the handler
            auto data = eventArgs.get<Component::EventArgs<>>();
            handler.get<Component::EventHandler<>>()->callback(*data);

        }
        // remove eventArgs from handler
        for (auto &&id : set) {
            handler.get()->destroyComponent(id);
        }
    }
}

void Engine::EventSystem::registerHandler(const Component::ComponentId &handler) {
    registeredHandlers.emplace(handler);
}
