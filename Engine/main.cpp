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
#include "ReportCard.h"

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

	auto debug = Engine::addSystem<Debug::LiveReloadSystem>();
	//debug->watch("tweaks.txt");

	// region initialize game-clocks
	using namespace std::chrono;
	typedef duration<float> floatMilliseconds; // define this to get float values;
	auto start = high_resolution_clock::now();
	auto end = start + milliseconds(1); // do this so physx doesn't complain about time being 0.
	// endregion

	auto player = Engine::getStore().getRoot().getComponentsOfType<ControlledVehicle>()[0];
	auto cb = [player](const Component::EventArgs<std::string>& args) {
		auto filename = std::get<0>(args.values);

		std::ifstream ifs(filename);

		json j;
		ifs >> j;		

		Engine::log<::module, Engine::high>("Updating vehicle params");

		auto engine = player->vehicle->pxVehicle->mDriveSimData.getEngineData();
		auto clutch = player->vehicle->pxVehicle->mDriveSimData.getClutchData();
		auto gears = player->vehicle->pxVehicle->mDriveSimData.getGearsData();

		for (int i = 0; i < 4; ++i) {
			auto wheels = player->vehicle->pxVehicle->mWheelsSimData.getTireData(i);
			wheels.mLongitudinalStiffnessPerUnitGravity = j["tires"]["mLongitudinalStiffnessPerUnitGravity"];
			wheels.mLatStiffX  = j["tires"]["mLatStiffX"];
			wheels.mLatStiffY  = j["tires"]["mLatStiffY"];
			player->vehicle->pxVehicle->mWheelsSimData.setTireData(i, wheels);
		}
		engine.mPeakTorque = j["engine"]["mPeakTorque"];
		engine.mMOI = j["engine"]["mMOI"];
		engine.mMaxOmega = j["engine"]["mMaxOmega"];
		engine.mDampingRateFullThrottle = j["engine"]["mDampingRateFullThrottle"];
		engine.mDampingRateZeroThrottleClutchEngaged = j["engine"]["mDampingRateZeroThrottleClutchEngaged"];
		engine.mDampingRateZeroThrottleClutchDisengaged = j["engine"]["mDampingRateZeroThrottleClutchDisengaged"];

		clutch.mStrength = j["clutch"]["mStrength"];
		clutch.mAccuracyMode = j["clutch"]["mAccuracyMode"];
		clutch.mEstimateIterations = j["clutch"]["mEstimateIterations"];

		for (auto&[key, value] : j["engine"]["gears"].items()) {
			gears.mRatios[value["index"]] = value["ratio"];
		}

		player->vehicle->pxVehicle->mDriveSimData.setEngineData(engine);
		player->vehicle->pxVehicle->mDriveSimData.setClutchData(clutch);
		player->vehicle->pxVehicle->mDriveSimData.setGearsData(gears);
	};
	std::shared_ptr<Component::EventHandler<std::string>> tweakHandler = Engine::EventSystem::createHandler<std::string>(std::function(cb));
	debug->onAssetModified += tweakHandler;
	 
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
