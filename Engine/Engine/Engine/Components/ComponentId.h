//
// Created by root on 11/1/20.
//

#ifndef ENGINE_COMPONENTID_H
#define ENGINE_COMPONENTID_H

#include <ostream>

namespace Component {

    class ComponentInterface;

    extern unsigned int next_id;

    struct ComponentId {

        unsigned int id;

        ComponentId();
        ComponentId(const ComponentId& other);
        ComponentId(bool, unsigned int) noexcept;

        ComponentId Create();

        explicit ComponentId(bool) noexcept;
        bool operator < (const ComponentId& other) const;

        std::shared_ptr<ComponentInterface> get();

        template<typename T>
        std::shared_ptr<T> get() {
            return Index::entityData<T>(*this);
        }

        friend std::ostream& operator<<(std::ostream& out, const Component::ComponentId& id);
    };

    enum class ClassId : unsigned int {
        Camera = 0xBEEF0000,
        Shader,
        Mesh,
        GameObject,
        Dirty,
        Event,
        EventArgs,
        EventHandler,
        Null = 0xFFFFFFFF
    };

    std::ostream& operator<<(std::ostream& out, const Component::ClassId& id);
}

#endif //ENGINE_COMPONENTID_H
