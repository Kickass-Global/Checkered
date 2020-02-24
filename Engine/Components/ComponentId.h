//
// Created by root on 11/1/20.
//

#pragma once

#ifndef ENGINE_COMPONENTID_H
#define ENGINE_COMPONENTID_H

#include <ostream>
#include <memory>
#include <set>

namespace Component {

	class ComponentInterface;

	extern unsigned int next_id;

	enum class ClassId : unsigned int {
		Camera = 0xBEEF0000,
		Tag,
		Shader,
		Mesh,
		Model,
		GameObject,
		SceneComponent,
		PhysicsPacket,
		Dirty,
		Damage,
		Vehicle,
		Transform,
		Program,
		Event,
		EventArgs,
		EventHandler,
		Billboard,
		None = 0xFFFFFFFF
	};

	struct ComponentId {

		enum {
			UNIQUE_ID = 1
		};

		static const unsigned int Null = 0xffffffffu;

		unsigned int id;

		// creates a null-like id
		ComponentId();
		ComponentId(const ComponentId& other);
		explicit ComponentId(bool) noexcept;

		bool operator<(const ComponentId& other) const;

		operator bool() const noexcept {
			return id != Null;
		}

		[[nodiscard]] ComponentInterface* base() const;
		[[nodiscard]] Component::ClassId classId() const;

		template<typename T>
		[[nodiscard]] T* data() const;

		friend std::ostream&
			operator<<(std::ostream& out, const Component::ComponentId& id);

		template<typename T>
		void addTag() const;

		template<typename T>
		[[nodiscard]] bool hasTag() const;

		void attachExistingComponent(const Component::ComponentId& componentId) const;
		void destroyComponent(const Component::ComponentId& componentId) const;
		void destroyComponentsOfType(Component::ClassId classId) const;

		[[nodiscard]] std::set<Component::ComponentId> childComponentsOfClass
		(Component::ClassId classId) const;

        void attachTemporaryComponent(const ComponentId& componentId, int ttl) const;
    };

	std::ostream& operator<<(std::ostream& out, const Component::ClassId& id);
}



#endif //ENGINE_COMPONENTID_H
