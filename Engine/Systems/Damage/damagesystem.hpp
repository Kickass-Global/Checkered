//
// Created by Jackson Cougar Wiebe on 1/27/2020.
//

#ifndef ENGINE_DAMAGESYSTEM_HPP
#define ENGINE_DAMAGESYSTEM_HPP


#include <numeric>
#include <algorithm>
#include "../../Engine.h"
#include "../../Components/damage.hpp"
#include "../../Components/EventHandler.h"
#include "../Events/Events.h"
#include "../../Components/ComponentEvent.h"
#include "../systeminterface.hpp"

namespace Engine {


    class ModelSystem : public Engine::SystemInterface {

        void update(Engine::deltaTime elapsed) override {
            // if the model has changed variations...

        }

    };

    class DamageSystem : public Engine::SystemInterface {

        void update(Engine::deltaTime elapsed) override {

            auto models = Component::Index::entitiesOf(
                    Component::ClassId::Model);

            for (auto model : models) {
                // check to see if this model has received any damage...
                auto damages = model.childComponentsOfClass
                        (Component::ClassId::Damage);

                if (!damages.empty()) {
                    Engine::log<module>("Updating model#", model);
                    auto &&meta = model.data<Component::Model>();
                    int total_damage = 0;
                    for (auto &damage : damages) {

                        total_damage += damage.data<Component::Damage>()
                                ->damage_amount;
                        Engine::log<module>("Accumulating damage#",
                                            total_damage);
                    }
                    meta->current_damage += total_damage;
                    auto current_damage = meta->current_damage;
                    Engine::log<module>("Total damage is now = ",
                                        meta->current_damage);

                    for (auto &&part : meta->parts) {

                        auto it = std::find_if(
                                part.variations.begin(),
                                part.variations.end(),
                                [current_damage = meta->current_damage]
                                        (auto variation) {
                                    return variation.damage_threshold >
                                           current_damage;
                                });
                        if (it != part.variations.end()) {
                            part.active_variation
                                    = it - part.variations.begin();
                        }
                    }
                }

                for (auto &&damage : damages) {
                    model.destroyComponent(damage);
                }
            }
        }
    };
}

#endif //ENGINE_DAMAGESYSTEM_HPP
