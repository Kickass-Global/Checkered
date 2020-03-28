#ifndef SCENARIOS_TESTWORLD_H
#define SCENARIOS_TESTWORLD_H

#include "scenariointerface.hpp"
#include <Engine.h>
#include "rendering/text.hpp"
#include <optional>
#include <Billboard.h>


struct Timer : public ComponentBase {

	std::shared_ptr<Billboard> background;
	std::shared_ptr<Text> text;
	BoxModel bounds;

	Timer() {

		text = getEngine()->createComponent<Text>("OKay");
		text->color = { 0,0,1 };
		text->font_size = 10;
		text->vertical_align = eVerticalAlign::middle;
		text->align = eAlign::center;
		text->plot = { 0,0,200,40 };
		text->src = { 0,0 };
		text->dst = { 0,.7 };

		background = getEngine()->createComponent<Billboard>(
			getEngine()->createComponent<Texture>("Assets/Textures/Props.png"));
		background->plot = BoxModel{ 0,0,200,40 };
		background->src = { 0,0 };
		background->dst = { 0,.7 };
	}

	Engine::floatMilliseconds deadline = std::chrono::minutes(2);

	std::chrono::time_point<std::chrono::steady_clock> previous;
	std::chrono::time_point<std::chrono::steady_clock> now;

	void start() {
		now = std::chrono::steady_clock::now();
	}

	void tick() {

		previous = now;
		now = std::chrono::steady_clock::now();
		auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - previous);
		deadline -= elapsed_time;

		text->set_text(deadline.count());

		if (deadline < std::chrono::seconds(0)) {
			// do something
		}
	}

	Engine::floatMilliseconds get_deadline() {
		return deadline;
	}

};

class TestWorld : public Engine::ScenarioInterface {
public:
private:
public:
	std::shared_ptr<Timer> timer;
	TestWorld(Engine::EngineSystem* enginePtr);
private:
	void load() override;
	void update(deltaTime d) override {
		if (timer) timer->tick();

		auto fps = getEngine()->createMomentaryComponent<Component::Text>(
			"Frametime: " + std::to_string(d));
		fps->font_size = 4;
		fps->color = { 1,1,1 };
		fps->plot = { 10,10,640,480 };
		fps->vertical_align = eVerticalAlign::bottom;
		fps->align = eAlign::left;
	}
};

#endif //SCENARIOS_TESTWORLD_H