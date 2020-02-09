//
// Created by root on 11/1/20.
//

#ifndef ENGINE_COMPONENTID_H
#define ENGINE_COMPONENTID_H

#include <ostream>
#include <memory>
#include "Index.h"

namespace Component {

    class ComponentInterface;

    extern unsigned int next_id;

    enum class ClassId : unsigned int {
        Camera = 0xBEEF0000,
        Shader,
        Mesh,
        Model,
        GameObject,
        SceneComponent,
        PhysicsPacket,
        Dirty,
        Damage,
        Transform,
        Program,
        Event,
        EventArgs,
        EventHandler,
        Billboard,
        None = 0xFFFFFFFF
    };


    struct ComponentId {

        unsigned int id;

        ComponentId();
        ComponentId(const ComponentId& other);
        ComponentId(bool, unsigned int) noexcept;

        ComponentId Create();

        explicit ComponentId(bool) noexcept;

        bool operator<(const ComponentId &other) const;

        [[nodiscard]] ComponentInterface *interface() const;
        [[nodiscard]] Component::ClassId classId() const;
        template<typename T>
        [[nodiscard]] T *data() const;

        friend std::ostream &
        operator<<(std::ostream &out, const Component::ComponentId &id);
        void attachExistingComponent(Component::ComponentId componentId) const;
        void destroyComponent(Component::ComponentId componentId) const;
        void destroyComponentsOfType(Component::ClassId classId) const;

        ComponentId parent() { return Component::Index::parentOf(*this); }
        [[nodiscard]] std::set<Component::ComponentId> childComponentsOfClass
                (Component::ClassId classId) const;
        [[nodiscard]] bool hasChildComponent(const Component::ComponentId &componentId) const;

    };

    template<typename T>
    T *Component::ComponentId::data() const {
        return Index::entityData<T>(*this);
    }

    std::ostream& operator<<(std::ostream& out, const Component::ClassId& id);
}

#endif //ENGINE_COMPONENTID_H
