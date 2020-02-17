//
// Created by root on 15/1/20.
//

#ifndef ENGINE_COMPONENTBASE_H
#define ENGINE_COMPONENTBASE_H

#include "Index.h"
#include "ComponentId.h"

namespace Component {

    class ComponentInterface {
    public:
        [[nodiscard]] virtual Component::ComponentId id() const = 0;
        [[nodiscard]] virtual Component::ClassId classId() const = 0;
    };

    template<ClassId E>
    class ComponentBase : public ComponentInterface {

    protected:
        const static Component::ClassId m_class = E;
        Component::ComponentId m_id;

    public:

        [[nodiscard]] Component::ComponentId id() const override;
        [[nodiscard]] Component::ClassId classId() const override;

        ComponentBase() : m_id(true) {}

        ComponentBase &operator=(ComponentBase &other) {
            if (this != &other) {
                // uh
            }
            return *this;
        }
    };

    class ComponentTagInterface {
        virtual Component::ComponentId id() = 0;
    };

    template<const char *name>
    class ComponentTag : public ComponentTagInterface {
    private:
        const static Component::ComponentId m_id;

    public:
        Component::ComponentId id() override { return m_id; }
        ComponentTag();
    };

    template<const char *name>
    Component::ComponentTag<name>::ComponentTag() {
        Engine::nameComponent(m_id, name);
    }

    template<const char *name>
    const Component::ComponentId Component::ComponentTag<name>::m_id = Component::ComponentId(ComponentId::UNIQUE_ID);


    template<Component::ClassId E>
    Component::ClassId ComponentBase<E>::classId() const { return m_class; }

    template<Component::ClassId E>
    Component::ComponentId ComponentBase<E>::id() const { return m_id; }

}

#endif //ENGINE_COMPONENTBASE_H
