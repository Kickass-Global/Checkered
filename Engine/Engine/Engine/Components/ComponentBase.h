//
// Created by root on 15/1/20.
//

#ifndef ENGINE_COMPONENTBASE_H
#define ENGINE_COMPONENTBASE_H

#include "ComponentId.h"
#include "Index.h"

namespace Component {

    class ComponentInterface {
    public:
        [[nodiscard]] virtual Component::ComponentId id() const  = 0;
        [[nodiscard]] virtual Component::ClassId classId() const = 0;
        virtual void attachComponent(Component::ComponentId id) const = 0;
        virtual void destroyComponent(Component::ComponentId id) const = 0;
        virtual void clone(Component::ComponentId id) = 0;
    };

    template<ClassId E>
    class ComponentBase : public ComponentInterface {

    protected:
        const static Component::ClassId m_class = E;
        Component::ComponentId m_id;

    public:

        [[nodiscard]] Component::ComponentId id() const override;
        [[nodiscard]] Component::ClassId classId() const override;
        void attachComponent(Component::ComponentId id) const override;
        void destroyComponent(Component::ComponentId id) const override;
        void clone(Component::ComponentId id) override;
        ComponentBase() : m_id(true) {}
    };

    template<Component::ClassId E>
    Component::ClassId ComponentBase<E>::classId() const { return m_class; }

    template<Component::ClassId E>
    void ComponentBase<E>::attachComponent(Component::ComponentId id) const {
        Component::Index::addComponent(m_id, id);
    }

    template<Component::ClassId E>
    void ComponentBase<E>::destroyComponent(Component::ComponentId id) const {
        Component::Index::removeComponent(m_id, id);
    }

    template<Component::ClassId E>
    Component::ComponentId ComponentBase<E>::id() const { return m_id; }

    template<Component::ClassId E>
    void ComponentBase<E>::clone(Component::ComponentId id) {
        m_id = id;
    }
}

#endif //ENGINE_COMPONENTBASE_H
