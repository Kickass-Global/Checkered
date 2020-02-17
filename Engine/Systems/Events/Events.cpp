//
// Created by root on 20/1/20.
//

#include "Events.h"

std::set<Component::ComponentId> Engine::EventSystem::registeredHandlers;

Component::EventDelegate<Engine::deltaTime> Engine::EventSystem::onTick("onTick");

void Engine::EventSystem::update(Engine::deltaTime time) {

    // todo: combine EventArgs between updates so that handlers don't get showered with events. #optimization

    onTick(time);

    for (Component::ComponentId handler : registeredHandlers) {

        std::set<Component::ComponentId> set = handler.childComponentsOfClass(Component::ClassId::EventArgs);
        for (Component::ComponentId eventArgs : set) {

            // I want to take all the data and pass it to the callback of the handler
            auto data = eventArgs.data<Component::EventArgs<>>();
            handler.data<Component::EventHandler<>>()->callback(*data);

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