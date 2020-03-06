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
#include <sstream>
#include <algorithm>
#include <WorldTransform.h>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"

#include "ComponentId.h"
#include "tags.h"
#include "systeminterface.hpp"
#include "EngineStore.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#undef assert

using namespace Component; // import this into anything using the engine header

namespace Component {
	template<typename... Args>
	class EventDelegate;

	template<typename... As>
	class EventArgs;
}


namespace Engine {

	/**
	 * Used to keep track of which systems are currently running in the engine.
	 */
	extern std::vector<std::unique_ptr<Engine::SystemInterface>> registeredSystems;
	extern EngineStore store;

	typedef float deltaTime;

	namespace {
		const char module[] = "Engine";
	}

	/**
	 * Returns collection of systems currently running in the engine.
	 */
	const std::vector<std::unique_ptr<Engine::SystemInterface>> &systems();

	inline EngineStore& getStore() {
		return store;
	}

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

	void sortSystems();

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
	inline std::shared_ptr<typename std::enable_if<std::is_base_of<Component::ComponentInterface, T>::value, T>::type>
		createComponent(Args &&... args) {
		return store.getRoot().addComponent(store.create<T>(args...));
	}


	template<typename T, typename... Args>
	inline std::shared_ptr<typename std::enable_if<std::is_base_of<Component::ComponentInterface, T>::value, T>::type>
		createNamedComponent(std::string name, Args... args) {
		auto result = createComponent<T>(args...);
		Engine::nameComponent(result->getId(), name);
		return result;
	}

	template<typename T, typename... Args>
	inline std::shared_ptr<typename std::enable_if<std::is_base_of<Component::ComponentInterface, T>::value, T>::type>
		createComponentWithTTL(int ttl, const Args &... args) {
		return createComponent<T>(args...);
	}


	struct Name {
		std::string value;

		explicit Name(const char *module_name) : value(module_name) {}
	};

	std::ostream &operator<<(std::ostream &out, const glm::vec3& vector);
	std::ostream& operator<<(std::ostream&  out, const glm::quat& quat);
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

			std::ostringstream buffer;

			buffer << Name(m);
			(buffer << ... << args);
			buffer << " [FAILURE]\n";

			std::cerr << buffer.str() << std::flush;
			OutputDebugString(buffer.str().c_str());

			throw;
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

			std::ostringstream buffer;

			buffer << Name(m);
			(buffer << ... << args);
			buffer << "\n";

			std::cout << buffer.str() << std::flush;
			OutputDebugString(buffer.str().c_str());
		}
	}

}

#endif //ENGINE_ENGINE_H
