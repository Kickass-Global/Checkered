//
// Created by root on 11/1/20.
//

#ifndef ENGINE_COMPONENTID_H
#define ENGINE_COMPONENTID_H

#include <ostream>
#include "Index.h"

namespace Component {

    inline static int next_id = 1;

    struct ComponentId {
        int id;

        ComponentId() : id(-1) {        }
        ComponentId(const ComponentId& other);

        ComponentId Create();

        ComponentId(bool) noexcept;

        bool operator < (const ComponentId& other) const;

        friend std::ostream& operator<<(std::ostream& out, const Component::ComponentId& id);
    private:
    };

    enum class ClassId : int {
        Camera,
        Shader,
        Mesh,
        GameObject


    };

    std::ostream& operator<<(std::ostream& out, const Component::ClassId& id);


    template <ClassId E>
    class ComponentBase {
        const static ClassId m_class = E;
        ComponentId m_id;

    public:

        [[nodiscard]] ComponentId id() const { return m_id; }
        [[nodiscard]] ClassId cid() const;

        void attachComponent(Component::ComponentId id);

        void detachComponent(Component::ComponentId id);

        explicit ComponentBase() : m_id(true) {}
    };

    template<ClassId E>
    ClassId ComponentBase<E>::cid() const { return m_class; }

    template<ClassId E>
    void ComponentBase<E>::attachComponent(Component::ComponentId id) {
        Component::Index::addComponent(m_id, id);
    }

    template<ClassId E>
    void ComponentBase<E>::detachComponent(Component::ComponentId id) {
        Component::Index::removeComponent(m_id, id);
    }
}




#endif //ENGINE_COMPONENTID_H
