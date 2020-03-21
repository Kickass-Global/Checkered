//
// Created by Jackson Cougar Wiebe on 3/13/2020.
//

#ifndef ENGINE_START_HPP
#define ENGINE_START_HPP

#include <Engine.h>
#include <material.hpp>
#include <Billboard.h>
#include <Mesh.h>
#include <Pipeline/Library.h>
#include <Camera/CameraSystem.h>
#include <Input/InputSystem.h>
#include <Rendering/fontsystem.hpp>
#include <Rendering/BillboardSystem.h>

#include "scenariointerface.hpp"
#include <Types.hpp>
#include <testworld.hpp>
#include <utility>

struct LayoutItem {
	Engine::BoxModelLayer margin = { 10, 10, 10, 10 };
	Engine::Rectangle box = { 0, 0, 400, 400 };
};

struct MenuState : ComponentBase {

	std::shared_ptr<Billboard> background;
	std::shared_ptr<Text> text;
	BoxModel bounds;

	void layout(Engine::Rectangle destination, Engine::RelativeAnchor anchor) {
		auto d = bounds.plot(destination, { 0, 0 }, { 0, 0 });
		if (background) {
			background->anchor = { 1, 1 };
			background->plot = d;
		}
		text->plot = BoxModel{ d.x, d.y, d.width, d.height }.plot(d, { 0, 0 }, { -1, -1 });
	}

	MenuState(std::shared_ptr<Texture> background_image, std::string menu_text)
		: background(getEngine()->createComponent<Billboard>(background_image)),
		text(getEngine()->createComponent<Text>(menu_text)),
		bounds{ 0, 0, 640, 100 } {
		text->vertical_align = eVerticalAlign::middle;
	}

	MenuState(std::string menu_text)
		: text(getEngine()->createComponent<Text>(menu_text)),
		bounds{ 0, 0, 640, 100 } {
		text->vertical_align = eVerticalAlign::middle;
	}
};

struct MenuItem : ComponentBase {
	std::vector<MenuState> menu_states;
	unsigned int current_state = 0;

	void select() {
		onSelected(0);
	}

	void layout(Engine::Rectangle destination, Engine::RelativeAnchor anchor) {
		for (auto& state : menu_states) {
			state.layout(destination, anchor);
		}
	}

	EventDelegate<int> onSelected{ "onSelected" };
};

struct MenuList : public ComponentBase {
	Engine::BoxModel box{ 0, 0, 640, 200 };
	std::vector<MenuItem> items;
	int selected_item_index = 0;
	bool active = true;

	MenuItem& current() { return items[selected_item_index]; }

	void layout(BoxModel destination) {

		auto dst = box.plot(destination.box, { 0,0 }, { 0,0 });
		auto boxes = dst.subdivide(items.size());
		auto item_box = boxes.begin();

		for (auto& item : items) {
			item.layout(*item_box, { 0, 0 });
			item_box++;
		}
	}

	int count() { return static_cast<int>(items.size()); };

	void select_previous() {
		selected_item_index = --selected_item_index % count();
	}

	void select_next() {
		selected_item_index = ++selected_item_index % count();
	}
};

struct MainMenu : public ComponentBase {
	std::shared_ptr<MenuList> menu;
	EventDelegate<int> onStart{ "onStart" };

	MainMenu() {
		menu = getEngine()->createComponent<MenuList>();
		MenuState default_menu_state("Press Start to Continue...");
		MenuState begin_menu_state("Begin Game");
		MenuItem press_start_item;
		MenuItem begin_game_item;

		press_start_item.menu_states = { default_menu_state };
		press_start_item.onSelected += [this](int) {
			log<high>("onStart");
			onStart(0);
		};

		begin_game_item.menu_states = { begin_menu_state };

		menu->items.push_back(press_start_item);
		menu->items.push_back(begin_game_item);
	}
};

struct MenuSystem : public SystemInterface {
	std::shared_ptr<EventHandler<int>> onKeyPressHandler;

	void initialize() override {
		onKeyPressHandler = getEngine()->getSubSystem<EventSystem>()
			->createHandler(this, &MenuSystem::onKeyPress);
	}

	void update(Engine::deltaTime elapsed) override {
		SystemInterface::update(elapsed);
		auto cameras = getEngine()->getSubSystem<EngineStore>()
			->getRoot()
			.getComponentsOfType<Component::Camera>();
		auto lists = getEngine()->getSubSystem<EngineStore>()
			->getRoot()
			.getComponentsOfType<MenuList>();

		for (auto& list : lists) {
			if (!list->active) continue;
			BoxModel screen{ 0, 0, cameras[0]->viewport.width, cameras[0]->viewport.height };
			list->layout(screen);
		}
	}

	void onKeyPress(const EventArgs<int>& args) {

		auto key = args.get<0>();
		auto lists = getEngine()->getSubSystem<EngineStore>()
			->getRoot()
			.getComponentsOfType<MenuList>();
		for (auto& list : lists) {
			if (!list->active) continue;
			if (key == GLFW_KEY_SPACE) {
				// select the current menu item
				list->current().select();
			}
			if (key == GLFW_KEY_UP) {
				list->select_previous();
			}
			if (key == GLFW_KEY_DOWN) {
				list->select_next();
			}
		}
	}
};
namespace Engine {
	class Start : public ScenarioInterface {
	public:
		explicit Start(EngineSystem* pSystem)
			: ScenarioInterface(pSystem) {}

		void unload() override {
			log<high>("onUnload");
			getEngine()->getSubSystem<EngineStore>()->getRoot().eraseAllComponents();
			getEngine()->getSubSystem<EventSystem>()->reset();
			getEngine()->getSubSystem<Rendering::RenderingSystem>()->reset();
			ScenarioInterface::unload();
		}

		std::shared_ptr<MainMenu> menu;

		void load() override {

			auto cameraSystem = getEngine()->addSubSystem<::Camera::CameraSystem>();
			auto renderingSystem = getEngine()->addSubSystem<Rendering::RenderingSystem>();
			auto menuSystem = getEngine()->addSubSystem<MenuSystem>();
			auto inputSystem = getEngine()->addSubSystem<Input::InputSystem>();
			renderingSystem->addSubSystem<Engine::BillboardSystem>();
			renderingSystem->addSubSystem<Engine::FontSystem>();

			renderingSystem->onWindowSizeChanged += cameraSystem->onWindowSizeHandler;
			inputSystem->onKeyPress += menuSystem->onKeyPressHandler;

			getEngine()->createComponent<Component::Camera>();

			menu = getEngine()->createComponent<MainMenu>();
			menu->onStart += [this](int) {
				log<high>("onStart");
				getEngine()->load_scenario<TestWorld>();
			};
		}

		void update(deltaTime d) override {
			auto fps = getEngine()->createMomentaryComponent<Component::Text>(
				"Frametime: " + std::to_string(d));
			fps->font_size = 4;
			fps->plot = { 0,0,640,480 };
			fps->vertical_align = eVerticalAlign::bottom;
			fps->align = eAlign::left;
		}
	};
}

#endif //ENGINE_START_HPP
