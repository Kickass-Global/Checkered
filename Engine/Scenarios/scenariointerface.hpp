//
// Created by Jackson Cougar Wiebe on 3/13/2020.
//

#ifndef ENGINE_SCENARIOINTERFACE_HPP
#define ENGINE_SCENARIOINTERFACE_HPP

namespace Engine {
	class ScenarioInterface {
	protected:
		friend class EngineSystem;
		EngineSystem* enginePtr;
	public:

		explicit ScenarioInterface(EngineSystem* enginePtr) : enginePtr(enginePtr) {}

		virtual void update(float time) {};

		EngineSystem* getEngine() { return enginePtr; }

		virtual void load() {};
		virtual void unload() {};
	};
}


#endif //ENGINE_SCENARIOINTERFACE_HPP
