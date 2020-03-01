//
// Created by root on 19/1/20.
//

#pragma once
#ifndef ENGINE_BILLBOARDSYSTEM_H
#define ENGINE_BILLBOARDSYSTEM_H

#include <vector>
#include <Billboard.h>
#include "Component.h"
#include "tags.h"
#include "Rendering.h"
#include "Engine.h"

namespace Rendering {

    class BillboardSystem {

        static Component::EventDelegate<Component::ComponentId> onBillboardChanged;

        static void update() {

            // look for billboard components and notify when they have been modified

            for (auto &&billboard : Component::Index::entitiesOf<Component::Billboard>()) {
                auto billboard_was_modified = billboard.hasTag<Component::Dirty>(true);
                if (billboard_was_modified) {
                    Engine::log<module>("Billboard was modified ", billboard);
                    onBillboardChanged(billboard);
                }
            }
        }

    };

}

#endif //ENGINE_BILLBOARDSYSTEM_H
