//
// Created by root on 18/1/20.
//

#pragma once

#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include <map>
#include <memory>
#include <SystemInterface.hpp>
#include <ComponentInterface.h>
#include <scenariointerface.hpp>
#include "EngineStore.h"

using namespace Component; // import this into anything using the engine header

namespace Component {
	template<typename... Args>
	class EventDelegate;

	template<typename... As>
	class EventArgs;
}
namespace Engine {

	extern class EngineSystem* current;

	class EngineSystem : public SystemInterface {
		EngineStore* store = nullptr;

	public:

		std::shared_ptr<ScenarioInterface> scenario;

		template <typename T>
		void load_scenario() {
			static_assert(std::is_base_of<ScenarioInterface, T>::value);
			getEngine()->pause();
			if (this->scenario) this->scenario->unload();
			this->scenario = std::make_shared<T>(this);
			this->scenario->load();
            getEngine()->unpause();
		}

		EngineSystem() {
			Engine::current = this;
		}

		void update(deltaTime time) override;
		void initialize() override;

		/**
		 * Adds an existing component to the engine; the component ownership is transferred to the engine.
		 * @tparam T the type of the component
		 * @param data the component data
		 * @return returns a valid pointer to the component in the engine.
		 */
		template<typename T>
		T* addComponent(std::unique_ptr<T> data) {

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
		inline std::shared_ptr<typename std::enable_if<std::is_base_of<Component::ComponentInterface, T>::value, T>::type> createComponent(
			Args&&... args) {

			return store->getRoot().addComponent(store->create<T>(args...));
		}

        /**
         * Creates a new component and adds it to the engine.
         * @tparam T the type of component to create.
         * @tparam Args the types of arguments to pass to the constructor of the component.
         * @param args the actual arguments to pass to the constructor of the component
         * @return returns a pointer to the created component.
         */
        template<typename T, typename... Args>
        inline std::shared_ptr<typename std::enable_if<std::is_base_of<Component::ComponentInterface, T>::value, T>::type> createExternalComponent(
            Args&&... args) {

            return store->getRoot().addExternalComponent<T>(store->create<T>(args...));
        }

		/**
		 * Creates a new component and adds it to the engine.
		 * @tparam T the type of component to create.
		 * @param name the name of the component (used for debugging).
		 * @tparam Args the types of arguments to pass to the constructor of the component.
		 * @param args the actual arguments to pass to the constructor of the component
		 * @return returns a pointer to the created component.
		 */
		template<typename T, typename... Args>
		inline std::shared_ptr<typename std::enable_if<std::is_base_of<Component::ComponentInterface, T>::value, T>::type> createNamedComponent(
			std::string name,
			Args&&... args) {

			auto result = createComponent<T>(args...);
			return result;
		}

		/**
		 * Creates and adds a new component to the engine that only exists for the current frame.
		 * @tparam T the type of component to create.
		 * @tparam Args the types of arguments to pass to the constructor of the component.
		 * @param args the actual arguments to pass to the constructor of the component
		 * @return returns a pointer to the created component.
		 */
		template<typename T, typename... Args>
		inline std::shared_ptr<typename std::enable_if<std::is_base_of<Component::ComponentInterface, T>::value, T>::type> createMomentaryComponent(
			Args&&... args) {

			return store->getRoot().addMomentaryComponent(store->create<T>(args...));
		}

        void update();
        void unpause();
        void pause();
    };

	typedef float deltaTime;
}

#endif //ENGINE_ENGINE_H
