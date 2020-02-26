//
// Created by root on 11/1/20.
//

#ifndef ENGINE_INDEX_H
#define ENGINE_INDEX_H

#include <map>
#include <set>
#include <memory>
#include <any>
#include <unordered_map>
#include <typeindex>
#include <tuple>

#include "ComponentId.h"
#include "systeminterface.hpp"

namespace std
{
	template<> struct hash<Component::ComponentId>
	{
		std::size_t operator()(Component::ComponentId const& s) const noexcept
		{
			return s.id;
		}
	};
}

namespace Component {
	enum class ClassId : unsigned int;

	class ComponentInterface;
}

namespace Component {

	struct Reference {
		bool allow_reuse = false;
		int count = 0;
		std::type_index type = typeid(Component::ClassId);
	};

	struct TTL {
		int ttl = 0;
		ComponentId owner;
	};


	typedef std::unordered_map<ClassId, std::set<ComponentId>> ClassMap;

	class Index : public Engine::SystemInterface {
	private:

		static std::unordered_map<Component::ComponentId, ClassMap> child_components;
		static ClassMap scene;

		static std::unordered_map<Component::ComponentId, std::unique_ptr<Component::ComponentInterface>> meta;

		static std::unordered_map<ComponentId, Reference> reference_count;
		static std::unordered_map<ComponentId, TTL> ttl;

	public:

		void update(Engine::deltaTime /*elapsed*/) override;
		void initialize() override;

		template<typename T>
		static void push_entity(Component::ClassId cid, Component::ComponentId id, std::unique_ptr<T> data) {
			static_assert(std::is_base_of<Component::ComponentInterface, T>::value);

			meta.emplace(id, std::move(data));
			scene[id.classId()].emplace(id);
			child_components[id] = {};
			reference_count[id] = { false, 0, typeid(T) };
		}

		template<typename T>
		static inline typename std::enable_if<std::is_base_of<Component::ComponentInterface, T>::value, T>::type
			* allocate() {
			static_assert(std::is_base_of<Component::ComponentInterface, T>::value);

			T* result = nullptr;
			for (auto cid : scene[T::ComponentClass()]) {

				if (reference_count[cid].allow_reuse && reference_count[cid].count <= 0) {
					reference_count[cid] = { false, 0, typeid(T) };
					result = static_cast<T*>(meta[cid].get());
					break;
				}

			}

			return result;
		}

		template<typename T>
		static const std::set<Component::ComponentId>&entitiesOf() {
			static_assert(std::is_base_of<Component::ComponentInterface, T>::value);
			return scene[T::ComponentClass()];
		}

		static std::set<Component::ComponentId> componentsOf
		(Component::ComponentId id, Component::ClassId classId);


		template<typename T>
		static void addTag(Component::ComponentId eid) {
			static_assert(std::is_base_of<Component::ComponentTagInterface, T>::value);

			auto tagId = T().id();
			Engine::log<module, Engine::low>("Adding tag#", tagId, "to component#", eid);
			child_components[eid][tagId.type].emplace(tagId);
		}

		template<typename T>
		static bool hasTag(Component::ComponentId eid, bool removeTag = true) {
			static_assert(std::is_base_of<Component::ComponentTagInterface, T>::value);

			auto tagId = T().id();
			auto result = child_components[eid][tagId.classId()].count(tagId) > 0;

			if (removeTag) {
				Engine::log<module, Engine::low>("Removing tag#", tagId, "to component#", eid);
				child_components[eid][tagId.type].erase(tagId);
			}

			return result;
		}

		static void addComponent(Component::ComponentId parent, Component::ComponentId child, int ttl = -1)
		{
			child_components[parent][child.type].emplace(child);
			reference_count[child].allow_reuse = true;
			reference_count[child].count++;
		}

		static void removeComponent(Component::ComponentId parent, Component::ComponentId child)
		{
			child_components[parent][child.type].erase(child);
			reference_count[child].count--;
		}


		template<class T>
		static T* entityData(const Component::ComponentId& componentId) {
			auto it = meta.find(componentId);

			auto result = it == meta.end() ? nullptr : (T*)it->second.get();
			return result;
		}
	};

}

#endif //ENGINE_INDEX_H
