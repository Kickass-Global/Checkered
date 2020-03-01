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
        MeshInstance,
        Model,
        Material,
        Texture,
        GameObject,
        SceneComponent,
        PhysicsPacket,
        Dirty,
        Damage,
        Vehicle,
        Scenery,
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

        ClassId type = ClassId::None;
        unsigned int id = Null;
        mutable void *data_ptr = nullptr;

        // creates a null-like id
        ComponentId();

        ComponentId(const ComponentId &other);

        explicit ComponentId(ClassId type) noexcept;

        ClassId classId() const;

        bool operator<(const ComponentId &other) const;

        ComponentId &operator=(const ComponentId &other);

        operator bool() const noexcept {
            return id != Null;
        }

        template<typename T>
        [[nodiscard]] T *data() const;

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


    std::ostream &operator<<(std::ostream &out, const Component::ClassId &id);

    /**
     * Restricts assignment to a single class of components.
     * @tparam type the ClassId of the component
     */
    template<ClassId type>
    struct ComponentReference {
        const static ClassId classId = type;
        ComponentId reference;

        explicit ComponentReference(const ComponentId reference) : reference(reference) {
            Engine::assertLog(classId == reference.type);
        }

        operator ComponentId() {
            return reference;
        }
    };

}

namespace std {
    template<>
    struct hash<Component::ComponentId> {
        std::size_t operator()(Component::ComponentId const &s) const noexcept {
            return s.id;
        }
    };
}

#endif //ENGINE_COMPONENTID_H
