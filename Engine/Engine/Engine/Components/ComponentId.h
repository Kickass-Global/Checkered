//
// Created by root on 11/1/20.
//

#ifndef ENGINE_COMPONENTID_H
#define ENGINE_COMPONENTID_H

namespace Component {
    static int next_id = 1;

    struct ComponentId {
        int id;

        ComponentId() : id(-1) {        }

        ComponentId Create() {
            return ComponentId(true);
        }

        bool operator < (const ComponentId& other) const {
            return this->id < other.id;
        }

    private:
        ComponentId(bool) : id(Component::next_id++) {}
    };
}


#endif //ENGINE_COMPONENTID_H
