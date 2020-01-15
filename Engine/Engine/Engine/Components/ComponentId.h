//
// Created by root on 11/1/20.
//

#ifndef ENGINE_COMPONENTID_H
#define ENGINE_COMPONENTID_H

#include <ostream>

namespace Component {


    static int next_id = 1;

    struct EntityId {
        int id;

        EntityId() : id(-1){}

        EntityId Create() {
            return EntityId(true);
        }

        EntityId(bool) : id(Component::next_id++) {}
    };

    struct ComponentId {
        int id;

        ComponentId() : id(-1) {        }
        ComponentId(const ComponentId& other) {
            id = other.id;
        }

        ComponentId Create() {
            return ComponentId(true);
        }

        ComponentId(bool) noexcept : id(Component::next_id++) {}

        bool operator < (const ComponentId& other) const {
            return this->id < other.id;
        }

        friend std::ostream& operator<<(std::ostream& out, const Component::ComponentId& id) {
            return out << id.id;
        }
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

        ComponentId id() const { return m_id; }
        ClassId cid() const { return m_class; }

        void attachComponent(Component::ComponentId id) {
            Component::Index::addComponent(m_id, id);
        }

        void detachComponent(Component::ComponentId id) {
            Component::Index::removeComponent(m_id, id);
        }

        explicit ComponentBase() : m_id(true) {}
    };
}




#endif //ENGINE_COMPONENTID_H
