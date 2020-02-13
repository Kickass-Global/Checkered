//
// Created by Jackson Cougar Wiebe on 1/27/2020.
//

#ifndef ENGINE_DAMAGESYSTEM_HPP
#define ENGINE_DAMAGESYSTEM_HPP


#include <numeric>
#include <algorithm>

#include "Engine.h"
#include "../../Components/damage.hpp"
#include "../../Components/EventHandler.h"
#include "../Events/Events.h"
#include "../../Components/ComponentEvent.h"
#include "../../Systems/systeminterface.hpp"
#include "../systeminterface.hpp"
#include "../../Components/Dirty.h"

namespace Engine {


class DamageSystem : public Engine::SystemInterface {

    void initialize() override {

    }

    void update(Engine::deltaTime elapsed) override {

        auto models = Component::Index::entitiesOf(
                Component::ClassId::Model
        );

        for (const auto &model : models) {

            auto &&meta = model.data<Component::Model>();

            const bool is_dirty = model.hasChildComponent(Component::Dirty::id());
            model.destroyComponent(Component::Dirty::id());

            const auto transform = model.childComponentsOfClass(Component::ClassId::Transform);
            auto has_transform = !transform.empty();
            //assertLog(has_transform, "Checking model has a world transform");

            if (is_dirty && has_transform) {
                Engine::log("Updating dirty model#", model);

                for (auto &&part : meta->parts) {

                    auto &&mesh = part.variations[part.active_variation].mesh;

                    mesh.attachExistingComponent(Component::Visible::id());
                    mesh.attachExistingComponent(Component::Dirty::id());
                    mesh.attachExistingComponent(*transform.begin());

                }
                model.destroyComponent(Component::Dirty::id());
            }

            // check to see if this model has received any damage...
            auto damages = model.childComponentsOfClass
                    (Component::ClassId::Damage);

            if (!damages.empty()) {

                Engine::log("Processing damage on model#", model);
                int total_damage = 0;
                for (auto &damage : damages) {

                    total_damage += damage.data<Component::Damage>()->damage_amount;
                    Engine::log<module>(
                            "Accumulating damage#",
                            total_damage
                    );
                }
                meta->current_damage += total_damage;
                auto current_damage = meta->current_damage;
                Engine::log(
                        "Total damage is now = ",
                        meta->current_damage
                );


            }

            for (auto &&part : meta->parts) {

                auto it = std::find_if(
                        part.variations.begin(),
                        part.variations.end(),
                        [current_damage = meta->current_damage]
                                (auto variation) {
                            return variation.damage_threshold > current_damage;
                        }
                );

                if (it != part.variations.end()) {
                    auto previous = part.active_variation;
                    part.active_variation = static_cast<int>(it - part.variations.begin());

                    auto &&mesh = part.variations[part.active_variation].mesh;

                    mesh.attachExistingComponent(Component::Visible::id());

					if(has_transform)
						mesh.attachExistingComponent(*transform.begin());

                    auto variation_changed = part.active_variation != previous;

                    if (variation_changed) {
                        Engine::log(
                                "Updating variation#",
                                part.active_variation
                        );
                        mesh.attachExistingComponent(Component::Dirty::id());
                    }

                }
            }

			for (auto&&t : transform) {
				model.destroyComponent(t);
			}

            for (auto &&damage : damages) {
                model.destroyComponent(damage);
            }
        }
    }
};
}

#endif //ENGINE_DAMAGESYSTEM_HPP
