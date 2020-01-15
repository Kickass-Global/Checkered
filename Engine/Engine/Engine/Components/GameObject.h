//
// Created by root on 15/1/20.
//

#ifndef ENGINE_GAMEOBJECT_H
#define ENGINE_GAMEOBJECT_H

#include "ComponentId.h"

namespace Component {

    class GameObject : public Component::ComponentBase<ClassId::GameObject> {
    };

}


#endif //ENGINE_GAMEOBJECT_H
