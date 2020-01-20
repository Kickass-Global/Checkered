//
// Created by root on 19/1/20.
//

#ifndef ENGINE_BILLBOARDSYSTEM_H
#define ENGINE_BILLBOARDSYSTEM_H

#include <vector>
#include "../../Components/Component.h"
#include "../../Components/Dirty.h"
#include "Rendering.h"

namespace Rendering {

    class BillboardSystem {

        static Component::ComponentEvent<Component::ComponentId> onBillboardChanged;

        static void update() {

            // look for billboard components and notify when they have been modified

            for(auto&& billboard : Component::Index::entitiesOf(Component::ClassId::Billboard))
            {
                auto billboard_was_modified = Component::Index::hasComponent(billboard, Component::Dirty::id());
                if(billboard_was_modified)
                {
                    Engine::log<module>("Billboard was modified ", billboard);
                    onBillboardChanged(billboard);
                }
            }
        }

    };

}

#endif //ENGINE_BILLBOARDSYSTEM_H
