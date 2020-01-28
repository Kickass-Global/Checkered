//
// Created by Jackson Cougar Wiebe on 1/27/2020.
//

#ifndef ENGINE_DAMAGESYSTEM_HPP
#define ENGINE_DAMAGESYSTEM_HPP


#include <numeric>
#include "../../Engine.h"
#include "../../Components/damage.hpp"
#include "../../Components/EventHandler.h"
#include "../Events/Events.h"
#include "../../Components/ComponentEvent.h"
#include "../systeminterface.hpp"

class DamageSystem : public Engine::SystemInterface {

    void update(Engine::deltaTime elapsed) {

        auto models = Component::Index::entitiesOf(Component::ClassId::Model);
        for (auto model : models) {
            // check to see if this model has received any damage...
            auto damages = model.childComponentsOfClass
                    (Component::ClassId::Damage);

            if (!damages.empty()) {
                auto &&meta = model.data<Component::Model>();
                int total_damage = 0;
                for (auto &damage : damages) {
                    total_damage += damage.data<Component::Damage>()
                            ->damage_amount;
                }
                meta->current_damage += total_damage;
            }
        }
    }
};


#endif //ENGINE_DAMAGESYSTEM_HPP
