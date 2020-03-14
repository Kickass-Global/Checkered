//
// Created by root on 15/1/20.
//
#pragma once

#ifndef ENGINE_COMPONENTBASE_H
#define ENGINE_COMPONENTBASE_H

#include <any>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <algorithm>
#include "ComponentInterface.h"
#include <Engine.h>

namespace Component {


    class ComponentBase : public ComponentInterface {
        friend class ::EngineStore;

    protected:
        struct Node children;

        Engine::EngineSystem *enginePtr = nullptr;

    public:
        ComponentId id = ++ ++next_id;

        [[nodiscard]] struct Node &getChildren() override { return children; }

        [[nodiscard]] ComponentId getId() const override { return id; }

        Engine::EngineSystem *getEngine() { return enginePtr; }

        template<typename T>
        void addChildComponent(std::shared_ptr<T> component) {

            children.addComponent(component);
        }

        void addChildComponents() {}

        template<typename T>
        void addChildComponents(std::shared_ptr<T> component) {

            children.addComponent(component);
        }

        template<typename T, typename Ts>
        void addChildComponents(std::shared_ptr<T> component, std::shared_ptr<T> components) {

            children.addComponent(component);
            addChildComponents(components);
        }


        template<typename T>
        void eraseChildComponentsOfType() {

            children.eraseComponentsOfType<T>();
        }


        template<typename T, typename... Args>
        void emplaceChildComponent(Args &&... args) {

            children.addComponent(getEngine()->getSubSystem<EngineStore>()->create<T>(args...));
        }

        ComponentBase &operator=(const ComponentBase &other) {

            if (this == &other)return *this;

            children = other.children;
            //id  = other.id; // don't change it
        }
    };

}

#endif //ENGINE_COMPONENTBASE_H
