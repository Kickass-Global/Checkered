//
// Created by root on 11/1/20.
//

#ifndef ENGINE_COMPONENTID_H
#define ENGINE_COMPONENTID_H

#include <ostream>
#include <memory>

namespace Component {

    class ComponentInterface;

    extern unsigned int next_id;

    enum class ClassId : unsigned int {
        Camera = 0xBEEF0000,
        Shader,
        Mesh,
        GameObject,
        Dirty,
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

        bool operator < (const ComponentId& other) const;

        [[nodiscard]] std::shared_ptr<ComponentInterface> data() const;

        [[nodiscard]] Component::ClassId classId() const;

        template<typename T>
        [[nodiscard]] std::shared_ptr<T> data() const;

        friend std::ostream& operator<<(std::ostream& out, const Component::ComponentId& id);
    };

    template<typename T>
    std::shared_ptr<T> Component::ComponentId::data() const {
        return Index::entityData<T>(*this);
    }

    std::ostream& operator<<(std::ostream& out, const Component::ClassId& id);
}

#endif //ENGINE_COMPONENTID_H
