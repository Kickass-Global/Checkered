//
// Created by Jackson Cougar Wiebe on 1/27/2020.
//

#pragma once

#ifndef ENGINE_DAMAGE_HPP
#define ENGINE_DAMAGE_HPP

#include "ComponentBase.h"
#include "Mesh.h"
#include <vector>

namespace Component {

	/**
	 * This component communicates damage between entities
	 */
	class Damage : public ComponentBase {
	public:
		int damage_amount;
		std::string name_of_region;
		Damage(int damageAmount, std::string region);
	};

        struct DamageRegion : public ComponentBase {
          std::string region_name;
          DamageRegion(const std::string &name_of_region)
              : region_name(name_of_region) {}
        };

	/**
	 * This component controls how entities change in response to damage and
	 * other effects.
	 */
	class Model : public ComponentBase {
	public:

		EventDelegate<int> onHealthChanged{ "onHealthChanged" };
		EventDelegate<std::string> onRegionDestroyed{ "onRegionDestroyed" };

		struct Variation {
			int damage_threshold;
			std::shared_ptr<Component::PaintedMesh> mesh;
		};

		struct Part {
			std::vector<Variation> variations = {};
			int active_variation;
			std::string region_name;
			int current_damage = 0;
			int max_damage = 10;
			bool apply_damage_to_health = false;
			bool is_destroyed = false;

			Variation& getActiveVariation() {
				return variations[active_variation];
			}
		};

		int health = 100;
		int max_health = 100;
		bool is_outdated = false;
		glm::mat4 transform;
		std::vector<Part> parts;

		Part& getRegionByName(std::string name) {
			return *std::find_if(parts.begin(), parts.end(), [&name](Part& part) { return part.region_name == name; });
		}
	};
} // namespace Component

#endif // ENGINE_DAMAGE_HPP
