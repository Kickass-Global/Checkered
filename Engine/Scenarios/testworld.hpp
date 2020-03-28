#ifndef SCENARIOS_TESTWORLD_H
#define SCENARIOS_TESTWORLD_H

#include "scenariointerface.hpp"
#include <Engine.h>
#include "Rendering/text.hpp"
#include <optional>
#include <Billboard.h>
#include <Vehicle.h>
#include <Pipeline/Library.h>
#include <WorldTransform.h>
#include <scenery.hpp>

namespace Component {
    struct WaypointArrow : public ComponentBase {
        std::shared_ptr<Component::PaintedMesh> m_mesh;
        glm::vec3 waypoint_location = {0,0,0};
        std::shared_ptr<Vehicle> target_vehicle;

        WaypointArrow() {
            const std::shared_ptr<Program> &basic_program = getEngine()->getSubSystem<Pipeline::Library>()
                ->getAsset<Program>("Assets/Programs/basic.json");
            auto material = getEngine()->createComponent<Component::Material>(basic_program);
            auto mesh = getEngine()->getSubSystem<Pipeline::Library>()
                ->getAsset<Mesh>("Assets/Meshes/arrow.obj");
            m_mesh = getEngine()->createComponent<PaintedMesh>(mesh, material);
        }

        void tick(float time) {

            auto waypoints = getEngine()->getSubSystem<EngineStore>()->getRoot().getComponentsOfType<Waypoint>();
            if(!waypoints.empty()) {
                waypoint_location = waypoints[0]->actor->position;
                auto direction = -glm::normalize(waypoint_location - target_vehicle->position);
                auto up = glm::vec3{0, 1, 0};
                auto p = glm::cross(direction, up);
                glm::mat4 T = glm::mat4(glm::vec4(p, 0),
                                        glm::vec4(up, 0),
                                        glm::vec4(direction, 0),
                                        glm::vec4{
                                            target_vehicle->position.x,
                                            target_vehicle->position.y + 2,
                                            target_vehicle->position.z,
                                            1
                                        });


                m_mesh->getStore().eraseComponentsOfType<WorldTransform>();
                m_mesh->getStore().emplaceComponent<WorldTransform>(T);
            }
        }
    };
}
struct Timer : public ComponentBase {

    std::shared_ptr<Billboard> background;
    std::shared_ptr<Text> text;
    BoxModel bounds;

    Timer() {

        text = getEngine()->createComponent<Text>("OKay");
        text->color = {0, 0, 1};
        text->font_size = 10;
        text->vertical_align = eVerticalAlign::middle;
        text->align = eAlign::center;
        text->plot = {0, 0, 200, 40};
        text->src = {0, 0};
        text->dst = {0, -.7};

        background = getEngine()->createComponent<Billboard>(getEngine()->createComponent<Texture>(
            "Assets/Textures/Props.png"));
        background->plot = BoxModel{0, 0, 200, 40};
        background->src = {0, 0};
        background->dst = {0, -.7};
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
    TestWorld(Engine::EngineSystem *enginePtr);
private:
    void load() override;

    void update(deltaTime d) override {
        if (timer) timer->tick();

        auto fps = getEngine()->createMomentaryComponent<Component::Text>(
            "Frametime: " + std::to_string(d));
        fps->font_size = 4;
        fps->color = {1, 1, 1};
        fps->plot = {10, 10, 640, 480};
        fps->vertical_align = eVerticalAlign::bottom;
        fps->align = eAlign::left;
    }
};

#endif //SCENARIOS_TESTWORLD_H