//
// Created by root on 15/1/20.
//

#ifndef ENGINE_COMPONENTBASE_H
#define ENGINE_COMPONENTBASE_H

#include "ComponentId.h"

namespace Component {

    template<ClassId E>
    class ComponentBase {
        const static Component::ClassId m_class = E;
        Component::ComponentId m_id;

    public:

        [[nodiscard]] Component::ComponentId id() const { return m_id; }

        [[nodiscard]] Component::ClassId cid() const;

        void attachComponent(Component::ComponentId id);

        void detachComponent(Component::ComponentId id);

        explicit ComponentBase() : m_id(true) {}
    };

    template<Component::ClassId E>
    Component::ClassId ComponentBase<E>::cid() const { return m_class; }

    template<Component::ClassId E>
    void ComponentBase<E>::attachComponent(Component::ComponentId id) {
        Component::Index::addComponent(m_id, id);
    }

    template<Component::ClassId E>
    void ComponentBase<E>::detachComponent(Component::ComponentId id) {
        Component::Index::removeComponent(m_id, id);
    }
}

#endif //ENGINE_COMPONENTBASE_H
