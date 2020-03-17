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

struct LayoutItem {
	Engine::BoxModel margin = { 10, 10, 10, 10 };
	Engine::Rectangle box = { 0, 0, 400, 400 };
};

struct MenuState : ComponentBase {

	std::shared_ptr<Billboard> background;
	std::shared_ptr<Text> text;
	Plot plot;

	void layout(Engine::Rectangle destination, Engine::RelativeAnchor anchor) {
		auto d = plot.plot_at(destination, anchor);
		background->anchor = { 1, 1 };
		background->plot = d.rectangle;
		text->plot = Engine::Plot{ {},{0,0,d.rectangle.width, d.rectangle.height} }.plot_at(d.rectangle, { 0,0 }).rectangle;
	}

	MenuState(std::shared_ptr<Texture> background_image, std::string menu_text)
		: background(getEngine()->createComponent<Billboard>(background_image)),
		text(getEngine()->createComponent<Text>(menu_text)) {

		// todo: layout the billboard and the text
		plot.rectangle = { 320,50, 640, 100 };
		text->vertical_align = eVerticalAlign::middle;
	}
};

struct MenuItem : ComponentBase {
	Engine::Rectangle box;
	std::vector<MenuState> menu_states;
	unsigned int current_state = 0;

	void select() {
		onSelected(0);
	}

	void layout(Engine::Rectangle destination, Engine::RelativeAnchor anchor) {
		box = destination;
		for (auto& state : menu_states) {
			state.layout(destination, anchor);
		}
	}

	EventDelegate<int> onSelected{ "onSelected" };
};

struct MenuList : public ComponentBase {
	Engine::Rectangle box;
	std::vector<MenuItem> items;
	int selected_item_index = 0;
	bool active = true;

	MenuItem& current() { return items[selected_item_index]; }

	void layout() {
		float item_height = box.height / items.size();
		int i = 0;
		for (auto& item : items) {
			auto item_box = Engine::Rectangle{ box.x, box.y + i * item_height, box.width, item_height };
			auto item_anchor = Engine::RelativeAnchor{ 0,0 }; // center
			item.layout(item_box, item_anchor);
			i++;
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
		MenuState default_menu_state(getEngine()->createComponent<Texture>(
			"Assets/Textures/Nature_Trees.png"), "Press Start to Continue...");
		MenuItem press_start_item;
		press_start_item.menu_states = { default_menu_state };
		press_start_item.onSelected += [this](int) {
			log<high>("onStart");
			onStart(0);
		};
		menu->items.push_back(press_start_item);
	}
};

struct MenuSystem : public SystemInterface {
	std::shared_ptr<EventHandler<int>> onKeyPressHandler;

	void initialize() override {
		onKeyPressHandler = getEngine()->getSubSystem<EventSystem>()->createHandler(this, &MenuSystem::onKeyPress);
	}

	void update(Engine::deltaTime elapsed) override {
		SystemInterface::update(elapsed);
		auto cameras = getEngine()->getSubSystem<EngineStore>()->getRoot().getComponentsOfType<Component::Camera>();
		auto lists = getEngine()->getSubSystem<EngineStore>()->getRoot().getComponentsOfType<MenuList>();

		for (auto& list : lists) {
			if (!list->active) continue;
			list->box = { 0,0, cameras[0]->viewport.width, cameras[0]->viewport.height };
			list->layout();
		}
	}

	void onKeyPress(const EventArgs<int>& args) {

		auto key = args.get<0>();
		auto lists = getEngine()->getSubSystem<EngineStore>()->getRoot().getComponentsOfType<MenuList>();
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
		explicit Start(EngineSystem* pSystem) : ScenarioInterface(pSystem) {}

        void unload() override {
			getEngine()->getSubSystem<EngineStore>()->getRoot().eraseAllComponents();
            ScenarioInterface::unload();
        }

        std::shared_ptr<MainMenu> menu;

        void load() override {

			auto cameraSystem = getEngine()->addSubSystem<::Camera::CameraSystem>();
			auto renderingSystem = getEngine()->addSubSystem<Rendering::RenderingSystem>();
			renderingSystem->onWindowSizeChanged += cameraSystem->onWindowSizeHandler;

			auto menuSystem = getEngine()->addSubSystem<MenuSystem>();
			auto inputSystem = getEngine()->addSubSystem<Input::InputSystem>();
			renderingSystem->addSubSystem<Engine::BillboardSystem>();
			renderingSystem->addSubSystem<Engine::FontSystem>();

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
		}
	};
}

#endif //ENGINE_START_HPP
