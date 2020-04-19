#include "testworld.hpp"
#include <functional>

#include "Camera/CameraSystem.h"
#include "Damage/damagesystem.hpp"
#include "Input/InputSystem.h"
#include "Passenger.h"
#include "Physics/PhysicsSystem.h"
#include <PxPhysicsAPI.h>
#include <Vehicle.h>
#include <Vehicle/vehiclesystem.hpp>

#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/transform.hpp"

#include "Component/scenecomponentsystem.hpp"
#include "Engine.h"
#include "WorldTransform.h"
#include <Billboard.h>
#include <PlayerVehicle.hpp>
#include <Rendering/BillboardSystem.h>
#include <Rendering/fontsystem.hpp>
#include <material.hpp>
#include <soundSystem/SoundSystem.h>
#include <texture.hpp>

void TestWorld::load() {
  auto cameraSystem = getEngine()->addSubSystem<::Camera::CameraSystem>();
  auto renderingSystem = getEngine()->addSubSystem<Rendering::RenderingSystem>();
  renderingSystem->onWindowSizeChanged += cameraSystem->onWindowSizeHandler;

  renderingSystem->addSubSystem<Engine::BillboardSystem>();
  renderingSystem->addSubSystem<Engine::FontSystem>();

  auto inputSystem = getEngine()->addSubSystem<Input::InputSystem>();

  auto physicsSystem = getEngine()->addSubSystem<Physics::PhysicsSystem>();
  auto vehicleSystem = getEngine()->addSubSystem<Engine::vehicleSystem>();
  auto openALSoundSystem = getEngine()->addSubSystem<Engine::SoundSystem>();
  auto scenarioLoader = getEngine()->addSubSystem<Pipeline::ScenarioLoader>();
  getEngine()->addSubSystem<PassengerSystem>();

  vehicleSystem->onVehicleCreated += physicsSystem->onVehicleCreatedHandler;

  auto componentSystem = getEngine()->addSubSystem<Component::SceneComponentSystem>();
  componentSystem->onActorCreated += physicsSystem->onActorCreatedHandler;

  getEngine()->addSubSystem<Engine::DamageSystem>();
  // auto renderingSystem =
  // getEngine()->addSubSystem<Rendering::RenderingSystem>();
  auto liveReloadSystem = getEngine()->addSubSystem<Debug::LiveReloadSystem>();

  // renderingSystem->addSubSystem<Engine::FontSystem>(); // yo dawg, I heard
  // you like systems in your systems.

  auto hornSystem = getEngine()->addSubSystem<Horn::hornSystem>();

  // hookup key press event with camera system
  auto eventSystem = getEngine()->addSubSystem<Engine::EventSystem>();
  eventSystem->order = 0;
  // renderingSystem->order = 2;

  inputSystem->onKeyPress += cameraSystem->onKeyPressHandler;
  inputSystem->onKeyDown += cameraSystem->onKeyDownHandler;
  inputSystem->onKeyUp += cameraSystem->onKeyUpHandler;

  inputSystem->onKeyPress += physicsSystem->onKeyPressHandler;
  inputSystem->onKeyDown += physicsSystem->onKeyDownHandler;
  inputSystem->onKeyUp += physicsSystem->onKeyUpHandler;

  inputSystem->onKeyPress += hornSystem->onKeyPressHandler;
  inputSystem->onKeyDown += hornSystem->onKeyDownHandler;
  inputSystem->onKeyUp += hornSystem->onKeyUpHandler;

  Rendering::RenderingSystem::onWindowSizeChanged += cameraSystem->onWindowSizeHandler;
  // endregion
  inputSystem->onKeyPress += openALSoundSystem->onKeyPressHandler;
  inputSystem->onKeyDown += openALSoundSystem->onKeyDownHandler;
  inputSystem->onKeyUp += openALSoundSystem->onKeyUpHandler;

  Rendering::RenderingSystem::onWindowSizeChanged += cameraSystem->onWindowSizeHandler;
  // endregion

  // setup the ground mesh

  auto basic_shader_program = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Program>(
      "Assets/Programs/basic.json");

 // timer = getEngine()->createComponent<Timer>();
  //timer->start();

  // create a scene object to hold the ground components to follow.
  Instance<DrivableScenery> drivable_instances;

  // make a material component
  auto ground_material_1 =
      getEngine()->createComponent<Component::Material>("Assets/Textures/bake_nw.png");
  auto ground_material_2 =
      getEngine()->createComponent<Component::Material>("Assets/Textures/bake_ne.png");
  auto ground_material_3 =
      getEngine()->createComponent<Component::Material>("Assets/Textures/bake_sw.png");
  auto ground_material_4 =
      getEngine()->createComponent<Component::Material>("Assets/Textures/bake_se.png");

  // load the mesh component
  auto city_block_1 =
      getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>("Assets/Meshes/city_nw.obj");
  auto city_block_2 =
      getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>("Assets/Meshes/city_ne.obj");
  auto city_block_3 =
      getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>("Assets/Meshes/city_sw.obj");
  auto city_block_4 =
      getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>("Assets/Meshes/city_se.obj");

  drivable_instances.add_instance_at(glm::translate(glm::vec3{0, 0, 0}), city_block_1,
                                     ground_material_1, city_block_1);
  drivable_instances.add_instance_at(glm::translate(glm::vec3{0, 0, 0}), city_block_2,
                                     ground_material_2, city_block_2);
  drivable_instances.add_instance_at(glm::translate(glm::vec3{0, 0, 0}), city_block_3,
                                     ground_material_3, city_block_3);
  drivable_instances.add_instance_at(glm::translate(glm::vec3{0, 0, 0}), city_block_4,
                                     ground_material_4, city_block_4);

  // create some buildings

  auto building_material1 = getEngine()->createComponent<Component::Material>(basic_shader_program);
  auto building_material2 = getEngine()->createComponent<Component::Material>(basic_shader_program);
  auto building_material3 = getEngine()->createComponent<Component::Material>(basic_shader_program);

  // create a sky...
  auto sky_shader_program =
      getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Program>("Assets/Programs/sky.json");

  auto sky_material = getEngine()->createComponent<Component::Material>(sky_shader_program);
  sky_material->textures.push_back(
      getEngine()->createComponent<Component::Texture>("Assets/Textures/CGSkies_0338_free.jpg"));
  auto skybox_mesh =
      getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>("Assets/Meshes/skybox.obj");

  auto sky = getEngine()->createComponent<PaintedMesh>(skybox_mesh, sky_material);
  sky->getStore().emplaceComponent<Component::WorldTransform>();

  building_material1->textures.push_back(
      getEngine()->createComponent<Component::Texture>("Assets/Textures/House_01.png"));
  building_material2->textures.push_back(
      getEngine()->createComponent<Component::Texture>("Assets/Textures/House_02.png"));
  building_material3->textures.push_back(
      getEngine()->createComponent<Component::Texture>("Assets/Textures/House_03.png"));

  auto building_mesh1 = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
      "Assets/Meshes/Building_House_01.fbx");
  auto building_mesh2 = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
      "Assets/Meshes/Building_House_013.fbx");
  auto building_mesh3 = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
      "Assets/Meshes/Building_Shop_02.fbx");

  // load scenario
  scenarioLoader->load_scenario<Obstacle>(*this, "Assets/Meshes/obstacles.dae");
  scenarioLoader->load_scenario<Scenery>(*this, "Assets/Meshes/static.dae");

  // setup a HUD element...

  auto billboard_mesh = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
      "Assets/Meshes/billboard_quad.obj");
  billboard_mesh->getStore().emplaceComponent<WorldTransform>();

  auto sprite = getEngine()->createComponent<Component::Billboard>();
  sprite->plot = BoxModel{10, 10, 100, 100};
  {
    auto material = getEngine()->createComponent<Component::Material>(
        getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Program>(
            "Assets/Programs/billboard.json"));

    material->textures.push_back(
        getEngine()->createComponent<Component::Texture>("Assets/Textures/Nature_Trees.png"));

    sprite->mesh_instance = getEngine()->createNamedComponent<PaintedMesh>(
        "billboard_mesh_instance", billboard_mesh, material);
  }

  // setup the mesh used for the cars...

  auto car_material = getEngine()->createComponent<Component::Material>(basic_shader_program);
  car_material->textures.push_back(
      getEngine()->createComponent<Component::Texture>("Assets/Textures/Vehicle_Car01_c.png"));
  auto car_mesh =
      getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>("Assets/Meshes/car_mesh.fbx");

  auto car_mesh_instance = getEngine()->createComponent<MeshInstance>(car_mesh, car_material);

  auto taxi_material = getEngine()->createComponent<Component::Material>(basic_shader_program);
  taxi_material->textures.push_back(
      getEngine()->createComponent<Component::Texture>("Assets/Textures/Vehicle_Taxi.png"));

  auto taxi_mesh =
      getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>("Assets/Meshes/taxi_mesh.fbx");

  // setup the player object....
  auto player = getEngine()->createComponent<Component::Player>();
  auto &player_vehicle = player->controller->vehicle;

  // setup passenger system

  Instance<Obstacle> obstacle_instances;
  auto dumpster_material = getEngine()->createComponent<Component::Material>(basic_shader_program);

  dumpster_material->textures.push_back(
      getEngine()->createComponent<Component::Texture>("Assets/Textures/Road_divider.png"));
  auto dumpster_mesh = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
      "Assets/Meshes/dumpster_mesh.fbx");

  auto hydrant_material = getEngine()->createComponent<Component::Material>(basic_shader_program);
  hydrant_material->textures.push_back(
      getEngine()->createComponent<Component::Texture>("Assets/Textures/Props.png"));
  auto hydrant_mesh = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
      "Assets/Meshes/Props_Buoy_02.fbx");

  auto tree_material = getEngine()->createComponent<Component::Material>(basic_shader_program);
  tree_material->textures.push_back(
      getEngine()->createComponent<Component::Texture>("Assets/Textures/Nature_Trees.png"));

  auto tree_mesh =
      getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>("Assets/Meshes/human.obj");

  glm::vec3 origin = {10, 0, 20};
  for (int i = 1; i < 10; i++) {
    obstacle_instances.add_instance_at(
        i / 50.0f * 200.0f * glm::vec3{std::cos(i), std::acos(i / 200.0), std::sin(i)} -
            glm::vec3{0, 5, 0} + origin,
        hydrant_mesh, hydrant_material);
  }

  using namespace Engine;

  // ai factory method...
  auto make_ai = [this, &taxi_mesh, &taxi_material](glm::mat4 world_transform = glm::mat4{1}) {
    auto ai_vehicle = getEngine()->createNamedComponent<Component::Taxi>("ai_vehicle");

    glm::quat orientation;
    glm::vec3 scale, translation, skew;
    glm::vec4 perspective;
    glm::decompose(world_transform, scale, orientation, translation, skew, perspective);

    ai_vehicle->player_vehicle->rotation = orientation;
    ai_vehicle->player_vehicle->position = translation;
    ai_vehicle->player_vehicle->local_position = glm::vec3(0.0f, -2.0f, 0.0f);

    return ai_vehicle;
  };

  // setup passenger
  // auto passenger_entity = Engine::createComponent<Component::Passenger>();
  // passenger_entity->initPassenger();

  // setup ai "brain"

  // setup ai "brain"
  std::function<void(const Component::EventArgs<Vehicle *> &)> ai_tick_callback =
      [player_vehicle](const Component::EventArgs<Vehicle *> &args) {
        auto meta = std::get<0>(args.values);

        if (!player_vehicle->pxVehicle) return;

        auto player_linVelo =
            player_vehicle->pxVehicle->getRigidDynamicActor()->getLinearVelocity();
        auto player_location =
            player_vehicle->position + glm::vec3(player_linVelo.x, 0,
                                                 player_linVelo.z);// translation vector of mat4
        auto ai_direction = glm::normalize(-meta->world_transform()[2]);// 'z' column vector of
                                                                        // mat4 (i.e. forward)
        auto ai_location = meta->position;// translation vector of mat4
        auto ai_velocity =
            (float) (pow(meta->pxVehicle->getRigidDynamicActor()->getLinearVelocity().x, 2)) +
            (float) (pow(meta->pxVehicle->getRigidDynamicActor()->getLinearVelocity().z, 2));

        auto player_distance = (float) (pow(ai_location.x - player_location.x, 2)) +
                               (float) (pow(ai_location.z - player_location.z, 2));
        // check if the player is to the left or right of the ai.
        auto perpdot = [](auto v1, auto v2) { return v1.z * v2.x - v1.x * v2.z; };

        meta->pxVehicleInputData.setAnalogSteer(0);

        meta->path.ReachedPoint(ai_location);

        if (player_distance < 4800.0f) {
          meta->path.ClearPathToGoal();

          auto player_direction =
              perpdot(glm::normalize(player_location - ai_location), ai_direction);
          // check if the player is in front or behind the ai.
          auto heading = [p = glm::normalize(glm::vec3(player_location - ai_location)),
                          b = glm::vec3(ai_direction)]() { return glm::dot(p, b); };

          const auto ai_is_driving_away = heading() < 0.0f;
          const auto player_is_left = player_direction > 0;

          if (ai_is_driving_away) {// turn the ai around...

            meta->pxVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
            meta->pxVehicle->mDriveDynData.setUseAutoGears(false);

            if (ai_velocity > 0.05f) meta->pxVehicleInputData.setAnalogAccel(0);
            else
              meta->pxVehicleInputData.setAnalogAccel(1);

            if (player_is_left) meta->pxVehicleInputData.setAnalogSteer(-1);
            else
              meta->pxVehicleInputData.setAnalogSteer(1);

          } else {// driving towards player

            meta->pxVehicle->mDriveDynData.setUseAutoGears(true);

            const auto pointed_at_player = -0.2f <= player_direction && player_direction <= 0.2f;

            if (pointed_at_player) {// drive straight
              meta->pxVehicleInputData.setAnalogAccel(1);
            } else {// steer towards player
              if (player_is_left) meta->pxVehicleInputData.setAnalogSteer(-1);
              else
                meta->pxVehicleInputData.setAnalogSteer(1);

              if (ai_velocity * 10 > player_distance && ai_velocity > 0.05f)
                meta->pxVehicleInputData.setAnalogAccel(0);
              else
                meta->pxVehicleInputData.setAnalogAccel(1);
            }
          }

        } else if (meta->path.pathToGoal.size() > 0) {
          auto playerPathChange = (float) (pow(meta->path.startNode->my_x - player_location.x, 2)) +
                                  (float) (pow(meta->path.startNode->my_z - player_location.z, 2));
          if (19200.0f < playerPathChange) {
            // Update the path using current path nodes as possible end points
            meta->path.FindPath(player_location, ai_location);
            meta->path.CleanPath();
          }

          auto nextPoint = meta->path.pathToGoal[0];
          auto player_distance = (float) (pow(ai_location.x - (nextPoint->my_x + 4.5f), 2)) +
                                 (float) (pow(ai_location.z - ((nextPoint->my_z + 4.5f)), 2));

          auto player_direction =
              perpdot(glm::normalize(glm::vec3(nextPoint->my_x + 4.5f, 0, nextPoint->my_z + 4.5f) -
                                     ai_location),
                      ai_direction);
          // check if the point is in front or behind the ai.
          auto heading =
              [p = glm::normalize(glm::vec3(nextPoint->my_x + 4.5f, 0, nextPoint->my_z + 4.5f) -
                                  ai_location),
               b = glm::vec3(ai_direction)]() { return glm::dot(p, b); };

          const auto ai_is_driving_away = heading() < 0.0f;
          const auto player_is_left = player_direction > 0;

          if (ai_is_driving_away) {// turn the ai around...
            meta->pxVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
            meta->pxVehicle->mDriveDynData.setUseAutoGears(false);

            if (ai_velocity > 0.05f) meta->pxVehicleInputData.setAnalogAccel(0);
            else
              meta->pxVehicleInputData.setAnalogAccel(1);

            if (player_is_left) meta->pxVehicleInputData.setAnalogSteer(-1);
            else
              meta->pxVehicleInputData.setAnalogSteer(1);

          } else {// driving towards point

            meta->pxVehicle->mDriveDynData.setUseAutoGears(true);

            const auto pointed_at_player = -0.2f <= player_direction && player_direction <= 0.2f;

            if (pointed_at_player) {
              if (ai_velocity * 50 > player_distance && ai_velocity > 0.05f) {
                meta->pxVehicleInputData.setAnalogAccel(0);
                // ready turn for next node if one exists
                if (meta->path.pathToGoal.size() > 1) {
                  nextPoint = meta->path.pathToGoal[1];

                  player_direction = perpdot(
                      glm::normalize(glm::vec3(nextPoint->my_x + 4.5f, 0, nextPoint->my_z + 4.5f) -
                                     ai_location),
                      ai_direction);
                  if (player_direction > 0.3) meta->pxVehicleInputData.setAnalogSteer(-1);
                  else if (player_direction < -0.3)
                    meta->pxVehicleInputData.setAnalogSteer(1);
                }

              } else {
                meta->pxVehicleInputData.setAnalogAccel(1);
              }

            } else {// steer towards player
              if (player_is_left) meta->pxVehicleInputData.setAnalogSteer(-1);
              else
                meta->pxVehicleInputData.setAnalogSteer(1);

              if (ai_velocity * 50 > player_distance && ai_velocity > 0.05f)
                meta->pxVehicleInputData.setAnalogAccel(0);
              else
                meta->pxVehicleInputData.setAnalogAccel(1);
            }
          }
        } else {
          meta->path.CheckInit = false;
          meta->path.FindPath(player_location, ai_location);
          meta->path.CleanPath();
        }
      };
  std::shared_ptr<EventHandler<Vehicle *>> ticker =
      getEngine()->getSubSystem<EventSystem>()->createHandler(ai_tick_callback);

  //   spawn some ai bois into the world
  auto dim = 1;
  int spacing = 40;
  for (int x = -dim; x <= dim; x++) {
    for (int y = dim; y <= dim; y++) {
      auto ai_vehicle = make_ai(glm::translate(glm::vec3(x * spacing, 30, y * spacing + 10)));
      ai_vehicle->player_vehicle->local_rotation = glm::rotate(3.14159f, glm::vec3(0, 1, 0));
      ai_vehicle->player_vehicle->path.graphNodes = nav;
      ai_vehicle->player_vehicle->tickHandler += ticker;// give them brain
    }
  }

  // make a default camera
  // auto camera =  getEngine()->createComponent<Component::Camera>();
  // camera->target = player_vehicle; // make camera follow player.
  // inputSystem->onGamePadStateChanged +=
  // cameraSystem->onGamepadStateChangedHandler;

  // region initialize game-clocks
  using namespace std::chrono;
  typedef duration<float> floatMilliseconds;// define this to get float values;
  auto start = high_resolution_clock::now();
  auto end = start + milliseconds(1);// do this so physx doesn't complain about
                                     // time being 0. endregion
}

TestWorld::TestWorld(EngineSystem *enginePtr) : ScenarioInterface(enginePtr) {}
