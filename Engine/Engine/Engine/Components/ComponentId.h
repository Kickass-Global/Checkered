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
}




#endif //ENGINE_COMPONENTID_H
