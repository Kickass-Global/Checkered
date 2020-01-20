//
// Created by root on 18/1/20.
//

#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include <map>
#include <memory>
#include <iostream>
#include <functional>
#include <vector>
#include "Components/ComponentId.h"
#include "Components/Index.h"
#include "SystemCalls.h"

namespace Component {
    template<typename... Args>
    class ComponentEvent;

    template<typename... As>
    class EventArgs;

    struct ComponentId;
    enum class ClassId : unsigned int;
}

namespace Engine {

    typedef float frametime;

    namespace {
        constexpr char module[] = "Engine";
        static std::map<Component::ComponentId, std::string> names;
    }

    static bool hasName(Component::ComponentId id)
    {
        return names.count(id) > 0;
    }

    static void name(Component::ComponentId id, std::string name)
    {
        names[id] = name;
    }

    template<typename T>
    std::shared_ptr<T> createComponent() {
        auto component = std::make_shared<T>();
        Component::Index::push_entity(component->classId(), component->id(), component);

        return component;
    }

    template<typename T>
    std::shared_ptr<T> createComponent(std::shared_ptr<T> component) {
        Component::Index::push_entity(component->classId(), component->id(), component);
        return component;
    }

    template<typename T>
    std::shared_ptr<T> replaceComponent(Component::ComponentId id, std::shared_ptr<T> component) {
        Component::Index::replace_component_data(id, component);
        auto result = Component::Index::entityData<T>(id);
        return result;

    }
}

#endif //ENGINE_ENGINE_H
