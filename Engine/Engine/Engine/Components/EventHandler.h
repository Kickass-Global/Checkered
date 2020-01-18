//
// Created by root on 17/1/20.
//

#ifndef ENGINE_EVENTHANDLER_H
#define ENGINE_EVENTHANDLER_H

#include <functional>

#include "ComponentId.h"
#include "ComponentBase.h"

namespace Component {

    template<typename... Args>
    class EventArgs;

    template<typename... Args>
    class EventHandler : public Component::ComponentBase<Component::ClassId::EventHandler> {
    public:
        std::function<void(const Component::EventArgs<Args...>&)> callback;
    };

}
#endif //ENGINE_EVENTHANDLER_H
