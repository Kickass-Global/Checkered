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
#include <optional>

struct LayoutItem {
    Engine::BoxModelLayer margin = {10, 10, 10, 10};
    Engine::Rectangle box = {0, 0, 400, 400};
};

template<typename T>
struct state {
    std::optional<T> previous;
    T now;

    state(T initial_value)
        : now(initial_value) {}

    state<T> &operator=(const T &value) {
        previous = now;
        now = value;

        return *this;
    }

    state<T>() = default;

    bool has_changed() {
        auto result = !previous || previous != now;
        previous = now;
        return result;
    }

    operator T() {
        return now;
    }
};

struct MenuState : ComponentBase {

    std::shared_ptr<Billboard> background;
    std::shared_ptr<Text> text;
    BoxModel bounds;
    state<bool> active;

    void layout(Engine::Rectangle destination, Engine::RelativeAnchor anchor) {
        if (active.has_changed()) {
            if (active) {
                getEngine()->getSubSystem<EngineStore>()->getRoot().activate(background.get());
                getEngine()->getSubSystem<EngineStore>()->getRoot().activate(text.get());

            } else {
                getEngine()->getSubSystem<EngineStore>()->getRoot().deactivate(background.get());
                getEngine()->getSubSystem<EngineStore>()->getRoot().deactivate(text.get());
            }
        }
        auto d = bounds.plot(destination, {0, 0}, {0, 0});
        if (background) {
            background->src = {1, 1};
            background->dst = {1, 1};
        }
        text->plot = BoxModel{d.x, d.y, d.width, d.height}.plot(d, {0, 1}, {-1, -1});
        text->src = {0, 0};
        text->dst = {0, 0};
        
    }

    MenuState(std::shared_ptr<Texture> background_image, std::string menu_text)
        : background(getEngine()->createComponent<Billboard>(background_image)),
          text(getEngine()->createComponent<Text>(menu_text)),
          bounds{0, 0, 640, 100} {
        text->vertical_align = eVerticalAlign::middle;
    }

    MenuState(std::string menu_text)
        : text(getEngine()->createComponent<Text>(menu_text)), bounds{0, 0, 640, 100} {
        text->vertical_align = eVerticalAlign::middle;
    }
};

struct MenuItem : ComponentBase {

    EventDelegate<int> onSelected{"onSelected"};
    MenuState selectedState;
    MenuState defaultState;
    state<int> current_state = 0;

    void deselect() {
        current_state = 0;
    }

    void select() {
        current_state = 1;
    }

    void activate() {
        onSelected(0);
    }

    void layout(Engine::Rectangle destination, Engine::RelativeAnchor anchor) {
        if (current_state.has_changed()) {
            if (current_state == 0) {
                selectedState.active = true;
                defaultState.active = false;
            } else {
                selectedState.active = false;
                defaultState.active = true;
            }
        }
        selectedState.layout(destination, anchor);
        defaultState.layout(destination, anchor);
    }

    MenuItem(MenuState normal, MenuState selected)
        : selectedState(selected), defaultState(normal) {}
};

struct MenuList : public ComponentBase {
    Engine::BoxModel box{0, 0, 640, 200};
    std::vector<MenuItem> items;
    state<int> selected_item_index = 0;
    bool active = true;

    MenuItem &current() { return items[selected_item_index]; }

    void layout(BoxModel destination) {
        if (selected_item_index.has_changed()) {
            for (int i = 0; i < items.size(); ++i) {
                if (i == selected_item_index) items[i].select();
                else items[i].deselect();
            }
        }
        auto dst = box.plot(destination.box, {0, 0}, {0, 0});
        auto boxes = dst.subdivide(static_cast<unsigned int>(items.size()));
        auto item_box = boxes.begin();

        for (auto &item : items) {
            item.layout(*item_box, {0, 0});
            item_box++;
        }
    }

    int count() { return static_cast<int>(items.size()); };

    unsigned modulo(int value, unsigned m) {
        int mod = value % (int) m;
        if (value < 0) {
            mod += m;
        }
        return mod;
    }

    void select_previous_item() {
        selected_item_index = modulo(selected_item_index - 1, count());
    }

    void select_next_item() {
        selected_item_index = modulo(selected_item_index + 1, count());
    }
};

struct MainMenu : public ComponentBase {
    std::shared_ptr<MenuList> menu;
    EventDelegate<int> onStart{"onStart"};

    MainMenu() {

        //start game button
        menu = getEngine()->createComponent<MenuList>();


        MenuItem start_item{ {"Get Checkered!"},{"Get Checkered!"} };
        start_item.defaultState.text->align = eAlign::left;
        start_item.selectedState.text->align = eAlign::left;
        start_item.selectedState.text->color = { 0,1,0 };
        start_item.onSelected += [this](int) {
            onStart(0);

        };
        menu->items.push_back(start_item);

        
        //quit game button
        MenuItem quit_item{ {"Quit"},{"Quit"} };
        quit_item.defaultState.text->align = eAlign::right;
        quit_item.selectedState.text->align = eAlign::right;
        quit_item.selectedState.text->color = { 1,0,0 };
        quit_item.onSelected += [this](int) {
            exit(0);
        };
        menu->items.push_back(quit_item);


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

        for (auto &list : lists) {
            if (!list->active) continue;
            BoxModel screen{1, 0, cameras[0]->viewport.width, cameras[0]->viewport.height};
            list->layout(screen);
        }
    }

    void onKeyPress(const EventArgs<int> &args) {

        auto key = args.get<0>();
        auto lists = getEngine()->getSubSystem<EngineStore>()
            ->getRoot()
            .getComponentsOfType<MenuList>();

        for (auto &list : lists) {
            if (!list->active) continue;
            if (key == GLFW_KEY_SPACE) {
                // select the current menu item
                list->select_next_item();
                list->current().activate();
            }
            if (key == GLFW_KEY_RIGHT) {
                list->select_previous_item();
            }
            if (key == GLFW_KEY_LEFT) {
                list->select_next_item();
            }
        }
    }


    void onGamePadStateChanged(const EventArgs<GLFWgamepadstate, GLFWgamepadstate>& args) {

        auto gamepad = std::get<0>(args.values);



    }
    
};
namespace Engine {
    class Start : public ScenarioInterface {
    public:
        explicit Start(EngineSystem *pSystem)
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
            fps->plot = {10, 10, 640, 480};
            fps->vertical_align = eVerticalAlign::bottom;
            fps->align = eAlign::left;
        }
    };
}

#endif //ENGINE_START_HPP
