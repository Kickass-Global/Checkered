//
// Created by Jackson Cougar Wiebe on 1/27/2020.
//

#ifndef ENGINE_DAMAGE_HPP
#define ENGINE_DAMAGE_HPP

#include <vector>
#include "ComponentBase.h"

namespace Component {

/**
 * This component communicates damage between entities
 */
    class Damage : public Component::ComponentBase<Component::ClassId::Damage> {
    public:
        int damage_amount;
    };

/**
 * This component controls how entities change in response to damage and
 * other effects.
 */
    class Model : public Component::ComponentBase<Component::ClassId::Model> {
    public:

        int current_damage;
        int max_damage;

    private:
        struct Variation {
            std::string name;
            int damage_threshold;
            Component::ComponentId mesh;
        };

        struct Part {
            std::string name;
            std::vector<Variation> variations;
        };

        std::vector<Part> parts;
    };
}

#endif //ENGINE_DAMAGE_HPP
