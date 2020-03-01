#pragma once
#ifndef ENGINE_COMPONENTS_SCENERY_HPP
#define ENGINE_COMPONENTS_SCENERY_HPP

#include <Engine.h>
#include <material.hpp>
#include <Mesh.h>
#include <SceneComponent.h>

namespace Component {

	/**
	 * Scenery components are things that can be placed into the world and have collision properties
	 */
	class Scenery : public SceneComponent {
	public:
		Scenery(const class Mesh& mesh, const Material &material)
		{
			const auto mesh_instance = Engine::createComponent<MeshInstance>(mesh, material);
			id().attachExistingComponent(mesh_instance->id());
		}
	};

}

#endif