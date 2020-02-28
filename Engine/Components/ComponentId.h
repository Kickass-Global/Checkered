//
// Created by root on 11/1/20.
//

#pragma once

#ifndef ENGINE_COMPONENTID_H
#define ENGINE_COMPONENTID_H

#include <ostream>
#include <memory>
#include <set>
#include <typeindex>

namespace Component {

	class ComponentInterface;

	extern unsigned int next_id;

    enum class ClassId : unsigned int {
        Camera = 0xBEEF0000,
        Shader,
        Tag,
        Mesh,
        Model,
        Material,
        Texture,
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
        Passenger,
        None = 0xFFFFFFFF
    };

    struct ComponentId {

        static const unsigned int Null = 0xffffffffu;

        ClassId type;
        unsigned int id;
        mutable void *data_ptr = nullptr;

        // creates a null-like id
        ComponentId();
        ComponentId(const ComponentId &other);
        explicit ComponentId(ClassId type) noexcept;

        ClassId classId() const;

        bool operator<(const ComponentId &other) const;

		operator bool() const noexcept {
			return id != Null;
		}

		template<typename T>
		[[nodiscard]] T* data() const;

        friend std::ostream &
        operator<<(std::ostream &out, const Component::ComponentId &id);

        template<typename T>
        void addTag() const;

        template<typename T>
        [[nodiscard]] bool hasTag(bool clear) const;

        void attachExistingComponent(ComponentId component) const;
        void destroyComponent(const Component::ComponentId &componentId) const;
        void destroyComponentsOfType(Component::ClassId classId) const;

        [[nodiscard]] std::set<Component::ComponentId> childComponentsOfClass
                (Component::ClassId classId) const;

        void attachTemporaryComponent(ComponentId componentId, int ttl) const;
    };

	std::ostream& operator<<(std::ostream& out, const Component::ClassId& id);
}



#endif //ENGINE_COMPONENTID_H
