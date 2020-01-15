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
}




#endif //ENGINE_COMPONENTID_H
