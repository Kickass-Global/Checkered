// Engine.cpp : Defines the entry point for the application.
//

#include <functional>

#include "main.h"
#include "Systems/Pipeline/EntityLoader.h"
#include "Systems/Damage/damagesystem.hpp"
#include <PxPhysicsAPI.h>
#include <Vehicle/vehiclesystem.hpp>
#include <Vehicle.h>
#include "Passenger.h"

#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include "Systems/Component/scenecomponentsystem.hpp"
#include "Systems/Navigation/astar.h"
#include "Systems/systeminterface.hpp"
#include "Engine.h"
#include "WorldTransform.h"
#include <Events/Events.h>
#include "soundSystem/SoundSystem.h"
#include <texture.hpp>
#include <fstream>
#include <material.hpp>
#include <Billboard.h>
#include "scenery.hpp"
#include "Scenarios/testworld.hpp"

int main() {

	// region initialize engine systems
	using namespace Engine;

	auto running = true; 

	TestWorld::load();
	
	auto cb = [](const Component::EventArgs<std::string>& args) {
		auto filename = std::get<0>(args.values);
		Engine::log(filename);
	};

	std::shared_ptr<Component::EventHandler<std::string>> tweakHandler = Engine::EventSystem::createHandler<std::string>(std::function(cb));

	auto debug = Engine::addSystem<Debug::LiveReloadSystem>();
	debug->watch("tweak.txt");
	debug->onAssetModified += tweakHandler;

	// region initialize game-clocks
	using namespace std::chrono;
	typedef duration<float> floatMilliseconds; // define this to get float values;
	auto start = high_resolution_clock::now();
	auto end = start + milliseconds(1); // do this so physx doesn't complain about time being 0.
	// endregion

	Engine::sortSystems();

	while (running) {

		// region before update
		floatMilliseconds delta = end - start;
		Engine::deltaTime elapsed = static_cast<Engine::deltaTime>(duration_cast<milliseconds>(delta).count());
		// endregion		

		Engine::EventSystem::onTick(elapsed);

		for (const auto& system : Engine::systems()) {
			system->update(elapsed);
		}

		// region after update
		start = end;
		end = high_resolution_clock::now();
		//endregion
	}
}
