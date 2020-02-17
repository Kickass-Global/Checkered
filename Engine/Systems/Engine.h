//
// Created by root on 18/1/20.
//

#pragma once

#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include <map>
#include <memory>
#include <iostream>
#include <functional>
#include <utility>
#include <vector>
#include <string>
#include <iomanip>
#include <WorldTransform.h>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"

#include "ComponentId.h"
#include "Index.h"
#include "tags.h"
#include "systeminterface.hpp"

#undef assert

namespace Component {
    template<typename... Args>
    class EventDelegate;

    template<typename... As>
    class EventArgs;

    struct ComponentId;
    enum class ClassId : unsigned int;
}

namespace Engine {

    /**
     * Used to keep track of which systems are currently running in the engine.
     */
    extern std::vector<std::unique_ptr<Engine::SystemInterface>> registeredSystems;

    typedef float deltaTime;

    namespace {
        const char module[] = "Engine";
    }

    /**
     * Returns collection of systems currently running in the engine.
     */
    const std::vector<std::unique_ptr<Engine::SystemInterface>> &systems();

    /**
     * Creates a new system to the engine; systems need to be default constructable.
     * @tparam T the type of system
     * @return returns a reference to the new system.
     */
    template<typename T>
    T *addSystem() {
        registeredSystems.push_back(std::make_unique<T>());
        registeredSystems.back()->initialize();
        return static_cast<T *>(registeredSystems.back().get());
    }

    /**
     * Users to keep track of 'human-readable' names for components; mostly for debugging purposes.
     */
    extern std::map<Component::ComponentId, std::string> identifier;

    /**
     * Gives a name to the component; useful for debugging purposes.
     * @param componentId the component to name
     * @param name the name of the component.
     */
    void nameComponent(const Component::ComponentId &componentId, std::string name);

    /**
     * Adds an existing component to the engine; the component ownership is transferred to the engine.
     * @tparam T the type of the component
     * @param data the component data
     * @return returns a valid pointer to the component in the engine.
     */
    template<typename T>
    T *addComponent(std::unique_ptr<T> data) {
        static_assert(std::is_base_of<Component::ComponentInterface, T>::value);

        auto id = data->id();
        auto classId = data->classId();

        Component::Index::push_entity(classId, id, std::move(data));

        id.addTag<Component::Dirty>();

        return id.data<T>();
    }

    /**
     * Creates a new component and adds it to the engine.
     * @tparam T the type of component to create.
     * @tparam Args the types of arguments to pass to the constructor of the component.
     * @param args the actual arguments to pass to the constructor of the component
     * @return returns a pointer to the created component.
     */

    template<typename T, typename... Args>
    inline typename std::enable_if<std::is_base_of<Component::ComponentInterface, T>::value, T>::type
    *createComponent(Args... args) {

        auto allocation = Component::Index::allocate<T>();
        if (allocation) {
            *allocation = T(args...);
            allocation->id().addTag<Component::Dirty>();
            return allocation;
        } else {

            auto component = std::make_unique<T>(args...);
            auto id = component->id();
            auto classId = component->classId();

            Component::Index::push_entity<T>(classId, id, std::move(component));
            id.addTag<Component::Dirty>();
            return id.data<T>();
        }
    }

    template<typename T, typename... Args>
    inline typename std::enable_if<std::is_base_of<Component::ComponentInterface, T>::value, T>::type
    *createComponentWithTTL(int ttl, Args... args) {

        T *result = Component::Index::allocate<T>();
        if (result) {
            *result = T(args...);
            result->id().addTag<Component::Dirty>();
        } else {

            auto component = std::make_unique<T>(args...);
            auto id = component->id();
            auto classId = component->classId();

            Component::Index::push_entity<T>(classId, id, std::move(component));
            id.addTag<Component::Dirty>();
            result = id.data<T>();
        }
        Component::Index::set_ttl(result->id(), ttl);
        return result;
    }


    struct Name {
        std::string value;

        explicit Name(const char *module_name) : value(module_name) {}
    };

    std::ostream &operator<<(std::ostream &out, Name name);

    /**
     * Asserts that given condition is successful and logs the given message.
     * @tparam module name to tag log messages with.
     * @param test the condition to test.
     * @param msg the message to log. (Should be in the form <msg> <SUCCEEDED>)
     */
    template<char const *m = module, class ... Ts>
    void assertLog(bool test, Ts...args) {

        if (!test) {

            std::cout << Name(m);
            (std::cout << ... << args);
            std::cout << " [FAILURE]" << std::endl;
            exit(-1);
        }

    }

    enum Importance {
        low,
        medium,
        high
    };

    constexpr Importance loggingLevel = high;

    template<char const *m = module, Importance importance = medium, class ... Ts>
    void log(Ts...args) {
        if (importance >= loggingLevel) {
            //std::cout << Name(m);
            //(std::cout << ... << args);
            //std::cout << std::endl;
        }
    }

}

#endif //ENGINE_ENGINE_H
