//
// Created by Jackson Cougar Wiebe on 1/27/2020.
//

#include "damagesystem.hpp"
#include <EngineStore.h>
#include <WorldTransform.h>

void Engine::DamageSystem::update(Engine::deltaTime elapsed) {

	auto models = getEngine()
		->getSubSystem<EngineStore>()
		->getRoot()
		.getComponentsOfType<Component::Model>();

	for (auto& model : models) {

		const bool is_dirty = model->is_outdated;

		if (is_dirty) {
			log<medium>("Updating dirty model#", model);
			model->is_outdated = false;
			for (auto&& part : model->parts) {

				auto&& mesh = part.variations[part.active_variation].mesh;
				if (mesh && !part.is_destroyed) {
					mesh->getStore().eraseComponentsOfType<Component::WorldTransform>();
					mesh->getStore().emplaceComponent<Component::WorldTransform>(model->transform);
				}
				else {
					mesh->getStore().eraseComponentsOfType<Component::WorldTransform>();
				}
			}
		}

		auto damages = model->getStore().getComponentsOfType<Damage>();

		std::map<std::string, size_t> index_of;

		size_t index = 0;
		for (auto&& part : model->parts) {
			index_of.emplace(part.region_name, index++);
		}

		bool health_has_changed = false;
		for (auto damage : damages) {
			auto i = index_of[damage->name_of_region];
			model->parts[i].current_damage += damage->damage_amount;
			if (model->parts[i].apply_damage_to_health) {
				model->health -= damage->damage_amount;
			}
			health_has_changed = true;
		}

		if (health_has_changed) {
			model->onHealthChanged(model->health);
		}


		model->getStore().eraseComponentsOfType<Damage>();

		for (auto&& part : model->parts) {

			if (part.is_destroyed) continue; // skip destroyed regions...

			auto it = std::find_if(
				part.variations.begin(), part.variations.end(),
				[current_damage = part.current_damage](auto variation) {
				return variation.damage_threshold > current_damage;
			});

			if (it != part.variations.end()) {
				auto previous = part.active_variation;
				part.active_variation = static_cast<int>(it - part.variations.begin());

				auto&& mesh = part.variations[part.active_variation].mesh;

				auto variation_changed = part.active_variation != previous;

				if (variation_changed) {
					Engine::log("Updating variation#", part.active_variation);
				}
			}
			if (part.current_damage > part.max_damage) // damage to region has exceeded all variation thresholds (region is destroyed).
			{
				part.is_destroyed = true;
				model->onRegionDestroyed(part.region_name);
			}
		}
	}
}

void Engine::DamageSystem::initialize() {}
