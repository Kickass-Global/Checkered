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


namespace Component {

	typedef int ComponentId;
	static ComponentId next_id = 800000000;

	class ComponentInterface {
	public:
		[[nodiscard]] virtual struct Node& getChildren() = 0;
		[[nodiscard]] virtual ComponentId getId() const = 0;

	};


	struct Node {
		std::unordered_map<std::type_index, std::vector<std::shared_ptr<class ComponentBase>>> components;

		template<typename T>
		std::vector<T*> getComponentsOfType() {
			static_assert(std::is_base_of<Component::ComponentInterface, T>::value);

			auto it = components.find(typeid(T));
			if (it == components.end()) return {};

			std::vector<T*> result;
			result.reserve(it->second.size());

			for (auto ref : it->second)
			{
				auto ptr = std::static_pointer_cast<T>(ref).get();
				result.push_back(ptr);
			}

			return result;
		}

		template<typename T>
		std::shared_ptr<T> addComponent(std::shared_ptr<T> component) {
			static_assert(std::is_base_of<Component::ComponentInterface, T>::value);

			components[typeid(T)].emplace_back(std::move(component));
			return std::dynamic_pointer_cast<T>(components[typeid(T)].back());
		}

		template<typename T>
		void eraseComponentsOfType() {
			static_assert(std::is_base_of<Component::ComponentInterface, T>::value);
			components.erase(typeid(T));
		}
		template<typename T>
		void eraseComponent(ComponentId id) {
			static_assert(std::is_base_of<Component::ComponentInterface, T>::value);

			auto it = std::find_if(
				components[typeid(T)].begin(),
				components[typeid(T)].end(),
				[id](auto elem) {return elem->getId() == id; }
			);

			if (it != components[typeid(T)].end()) components[typeid(T)].erase(it);
		}

	};


	class ComponentBase : public ComponentInterface {

	protected:
		struct Node children;

	public:
		ComponentId id = ++++next_id;

		[[nodiscard]] struct Node& getChildren() override { return children; }
		[[nodiscard]] ComponentId getId() const override { return id; }

		template <typename T>
		void addChildComponent(std::shared_ptr<T> component) {
			children.addComponent(component);
		}

		void addChildComponents() { }

		template <typename T>
		void addChildComponents(std::shared_ptr<T> component) {
			children.addComponent(component);
		}

		template <typename T, typename Ts>
		void addChildComponents(std::shared_ptr<T> component, std::shared_ptr<T> components) {
			children.addComponent(component);
			addChildComponents(components);
		}


		template <typename T>
		void eraseChildComponentsOfType()
		{
			children.eraseComponentsOfType<T>();
		}


		template <typename T, typename... Args>
		void emplaceChildComponent(Args& ... args) {
			children.addComponent(Engine::getStore().create<T>(args...));
		}

		ComponentBase &operator=(const ComponentBase &other) {
			if (this == &other)return *this;

			children = other.children;
			//id  = other.id; // don't change it
		}
	};

}

#endif //ENGINE_COMPONENTBASE_H
