//
// Created by root on 15/1/20.
//

#ifndef ENGINE_GAMEOBJECT_H
#define ENGINE_GAMEOBJECT_H

#include "ComponentId.h"
#include "ComponentBase.h"

namespace Component {

    class GameObject : public ComponentBase<ClassId::GameObject> {
    };

}


#endif //ENGINE_GAMEOBJECT_H
