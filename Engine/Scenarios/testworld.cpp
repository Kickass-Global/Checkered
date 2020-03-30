#include "testworld.hpp"
#include <functional>

#include "Camera/CameraSystem.h"
#include "Damage/damagesystem.hpp"
#include "Input/InputSystem.h"
#include "Passenger.h"
#include "Physics/PhysicsSystem.h"
#include "Pipeline/EntityLoader.h"
#include <PxPhysicsAPI.h>
#include <Vehicle.h>
#include <Vehicle/vehiclesystem.hpp>

#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/transform.hpp"

#include "Component/scenecomponentsystem.hpp"
#include "Engine.h"
#include "WorldTransform.h"
#include "scenery.hpp"
#include <Billboard.h>
#include <Rendering/BillboardSystem.h>
#include <Rendering/fontsystem.hpp>
#include <material.hpp>
#include <soundSystem/SoundSystem.h>
#include <texture.hpp>

void TestWorld::load() {
	auto cameraSystem = getEngine()->addSubSystem<::Camera::CameraSystem>();
	auto renderingSystem =
		getEngine()->addSubSystem<Rendering::RenderingSystem>();
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

	auto componentSystem =
		getEngine()->addSubSystem<Component::SceneComponentSystem>();
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

	Rendering::RenderingSystem::onWindowSizeChanged +=
		cameraSystem->onWindowSizeHandler;
	// endregion
	inputSystem->onKeyPress += openALSoundSystem->onKeyPressHandler;
	inputSystem->onKeyDown += openALSoundSystem->onKeyDownHandler;
	inputSystem->onKeyUp += openALSoundSystem->onKeyUpHandler;

	Rendering::RenderingSystem::onWindowSizeChanged +=
		cameraSystem->onWindowSizeHandler;
	// endregion

	// setup the ground mesh

	auto basic_shader_program =
		getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Program>(
			"Assets/Programs/basic.json");

	timer = getEngine()->createComponent<Timer>();
	timer->start();

	// create a scene object to hold the ground components to follow.
	Instance<DrivableScenery> drivable_instances;

	// make a material component
	auto ground_material =
		getEngine()->createComponent<Component::Material>(basic_shader_program);
	ground_material->textures.push_back(
		getEngine()->createComponent<Component::Texture>(
			"Assets/Textures/checkeredBowl.png"));
	ground_material->shader =
		getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Program>(
			"Assets/Programs/basic.json");

	// load the mesh component
	auto plane_mesh =
		getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
			"Assets/Meshes/checkeredBowl.fbx");
	drivable_instances.add_instance_at(
		glm::rotate(glm::radians(-90.0f), glm::vec3{ 1, 0, 0 }) *
		glm::translate(glm::vec3{ 0, -1, 0 }),
		plane_mesh, ground_material, plane_mesh);

	// create some buildings

	auto building_material1 =
		getEngine()->createComponent<Component::Material>(basic_shader_program);
	auto building_material2 =
		getEngine()->createComponent<Component::Material>(basic_shader_program);
	auto building_material3 =
		getEngine()->createComponent<Component::Material>(basic_shader_program);

	// create a sky...
	auto sky_material =
		getEngine()->createComponent<Component::Material>(basic_shader_program);
	sky_material->textures.push_back(
		getEngine()->createComponent<Component::Texture>(
			"Assets/Textures/CGSkies_0338_free.jpg"));
	auto skybox_mesh =
		getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
			"Assets/Meshes/skybox.obj");

	auto sky = getEngine()->createComponent<PaintedMesh>(skybox_mesh, sky_material);
	sky->getStore().emplaceComponent<Component::WorldTransform>();

	building_material1->textures.push_back(
		getEngine()->createComponent<Component::Texture>(
			"Assets/Textures/House_01.png"));
	building_material2->textures.push_back(
		getEngine()->createComponent<Component::Texture>(
			"Assets/Textures/House_02.png"));
	building_material3->textures.push_back(
		getEngine()->createComponent<Component::Texture>(
			"Assets/Textures/House_03.png"));

	auto building_mesh1 =
		getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
			"Assets/Meshes/Building_House_01.fbx");
	auto building_mesh2 =
		getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
			"Assets/Meshes/Building_House_013.fbx");
	auto building_mesh3 =
		getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
			"Assets/Meshes/Building_Shop_02.fbx");

	Instance<Scenery> building_instances;
	building_instances.add_instance_at(glm::vec3{ 43, 0, -60 }, building_mesh1,
		building_material1);
	building_instances.add_instance_at(glm::vec3{ 10, 0, -83 }, building_mesh2,
		building_material2);
	building_instances.add_instance_at(glm::vec3{ -20, 0, -86 }, building_mesh3,
		building_material3);
	building_instances.add_instance_at(glm::vec3{ 0, 0, -80 }, building_mesh3,
		building_material3);



	// load scenario
	// scenarioLoader->load_scenario(*this, "Assets/Meshes/city.dae");

	// setup a HUD element...

	auto billboard_mesh =
		getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
			"Assets/Meshes/billboard_quad.obj");
	billboard_mesh->getStore().emplaceComponent<WorldTransform>();

	auto sprite = getEngine()->createComponent<Component::Billboard>();
	sprite->plot = BoxModel{ 10, 10, 100, 100 };
	{
		auto material = getEngine()->createComponent<Component::Material>(
			getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Program>(
				"Assets/Programs/billboard.json"));

		material->textures.push_back(
			getEngine()->createComponent<Component::Texture>(
				"Assets/Textures/Nature_Trees.png"));

		sprite->mesh_instance = getEngine()->createNamedComponent<PaintedMesh>(
			"billboard_mesh_instance", billboard_mesh, material);
	}

	// setup the mesh used for the cars...

	auto car_material =
		getEngine()->createComponent<Component::Material>(basic_shader_program);
	car_material->textures.push_back(
		getEngine()->createComponent<Component::Texture>(
			"Assets/Textures/Vehicle_Car01_c.png"));
	auto car_mesh =
		getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
			"Assets/Meshes/car_mesh.fbx");

	auto car_mesh_instance = getEngine()->createNamedComponent<MeshInstance>(
		"car_mesh_instance", car_mesh, car_material);

	auto taxi_material =
		getEngine()->createComponent<Component::Material>(basic_shader_program);
	taxi_material->textures.push_back(
		getEngine()->createComponent<Component::Texture>(
			"Assets/Textures/Vehicle_Taxi.png"));

	auto taxi_mesh =
		getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
			"Assets/Meshes/taxi_mesh.fbx");

	// setup the vehicle for the player...

	auto player =
		getEngine()->createNamedComponent<Component::ControlledVehicle>("player");
	inputSystem->onGamePadStateChanged += player->onGamePadStateChangedHandler;
	inputSystem->onKeyUp += player->onKeyUpHandler;
	inputSystem->onKeyDown += player->onKeyDownHandler;

	auto& player_vehicle = player->vehicle;

	auto player_damage_model =
		getEngine()->createNamedComponent<Component::Model>(
			"player_damage_model");

	player_damage_model->parts.push_back(Component::Model::Part{});
	player_damage_model->parts[0].variations.push_back(
		Component::Model::Variation{
			2000000,
			getEngine()->createComponent<PaintedMesh>(car_mesh, car_material) });

	player_damage_model->onHealthChanged += [this](auto health) {
		if (health < 1) {
			log<high>("He's dead Jim");
			auto matte = getEngine()->createComponent<Billboard>(getEngine()->createComponent<Texture>("Assets/Textures/checkered-matte.png"));
			matte->dst = { 0,0 };
			matte->src = { 0,0 };
		}
	};

	player_vehicle->model = player_damage_model;
	player_vehicle->local_rotation = glm::rotate(3.14159f, glm::vec3(0, 1, 0));
	player_vehicle->local_position = glm::vec3(0.0f, -2.0f, 0.0f);
	player_vehicle->position = glm::vec3(0.0f, 3.0f, -40.0f);

	auto arrow = getEngine()->createComponent<WaypointArrow>();
	arrow->target_vehicle = player_vehicle;

	auto health_bar = getEngine()->createComponent<HealthBar>();
	health_bar->target = player_damage_model;

	getEngine()->getSubSystem<EventSystem>()->onTick +=
		std::bind(&WaypointArrow::tick, arrow, std::placeholders::_1);

	getEngine()->getSubSystem<EventSystem>()->onTick +=
		std::bind(&HealthBar::tick, health_bar, std::placeholders::_1);

	// setup passenger system

	Instance<Obstacle> obstacle_instances;
	auto dumpster_material =
		getEngine()->createComponent<Component::Material>(basic_shader_program);

	dumpster_material->textures.push_back(
		getEngine()->createComponent<Component::Texture>(
			"Assets/Textures/Road_divider.png"));
	auto dumpster_mesh =
		getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
			"Assets/Meshes/dumpster_mesh.fbx");

	auto hydrant_material =
		getEngine()->createComponent<Component::Material>(basic_shader_program);
	hydrant_material->textures.push_back(
		getEngine()->createComponent<Component::Texture>(
			"Assets/Textures/Props.png"));
	auto hydrant_mesh =
		getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
			"Assets/Meshes/Props_Buoy_02.fbx");

	auto tree_material =
		getEngine()->createComponent<Component::Material>(basic_shader_program);
	tree_material->textures.push_back(
		getEngine()->createComponent<Component::Texture>(
			"Assets/Textures/Nature_Trees.png"));

	auto tree_mesh =
		getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
			"Assets/Meshes/human.obj");

	glm::vec3 origin = { 10, 0, 20 };
	for (int i = 1; i < 10; i++) {
		obstacle_instances.add_instance_at(
			i / 50.0f * 200.0f *
			glm::vec3{ std::cos(i), std::acos(i / 200.0), std::sin(i) } -
			glm::vec3{ 0, 5, 0 } +origin,
			hydrant_mesh, hydrant_material);
	}

	using namespace Engine;

	// ai factory method...
	auto make_ai = [this, &taxi_mesh,
		&taxi_material](glm::mat4 world_transform = glm::mat4{ 1 }) {
		auto ai_vehicle =
			getEngine()->createNamedComponent<Component::Vehicle>("ai_vehicle");
		auto ai_damage_model =
			getEngine()->createNamedComponent<Component::Model>("ai_vehicle_model");

		ai_damage_model->parts.push_back(Component::Model::Part{});
		ai_damage_model->parts[0].variations.push_back(Component::Model::Variation{
			2000000,
			getEngine()->createComponent<PaintedMesh>(taxi_mesh, taxi_material) });

		glm::quat orientation;
		glm::vec3 scale, translation, skew;
		glm::vec4 perspective;
		glm::decompose(world_transform, scale, orientation, translation, skew,
			perspective);

		ai_vehicle->model = ai_damage_model;
		ai_vehicle->rotation = orientation;
		ai_vehicle->position = translation;
		ai_vehicle->local_position = glm::vec3(0.0f, -2.0f, 0.0f);

		return ai_vehicle;
	};

	// setup ai "brain"
	std::function<void(const Component::EventArgs<Vehicle*>&)>
		ai_tick_callback = [player_vehicle](
			const Component::EventArgs<Vehicle*>& args) {
				auto meta = std::get<0>(args.values);

				if (!meta->pxVehicle)
					return;

				auto player_location =
					player_vehicle->position; // translation vector of mat4
				auto ai_direction =
					glm::normalize(-meta->world_transform()[2]); // 'z' column vector of
																 // mat4 (i.e. forward)
				auto ai_location = meta->position; // translation vector of mat4

				// check if the player is to the left or right of the ai.
				auto perpdot = [](auto v1, auto v2) {
					return v1.z * v2.x - v1.x * v2.z;
				};

				meta->pxVehicleInputData.setAnalogSteer(0);

				// todo control throttle better...
				meta->pxVehicleInputData.setAnalogAccel(1);

				meta->path.ReachedPoint(ai_location);
				if (meta->path.pathToGoal.size() > 0) {
					auto nextPoint = meta->path.pathToGoal[0];
					auto player_direction =
						perpdot(glm::normalize(glm::vec3(nextPoint->my_x + 1.5f, 0,
							nextPoint->my_z + 1.5f) -
							ai_location),
							ai_direction);
					// check if the point is in front or behind the ai.
					auto heading =
						[p = glm::normalize(glm::vec3(nextPoint->my_x + 1.5f, 0,
							nextPoint->my_z + 1.5f) -
							ai_location),
						b = glm::vec3(ai_direction)]() { return glm::dot(p, b); };

					const auto ai_is_driving_away = heading() < 0.0f;
					const auto player_is_left = player_direction > 0;

					if (ai_is_driving_away) { // turn the ai around...

						meta->pxVehicle->mDriveDynData.forceGearChange(
							PxVehicleGearsData::eFIRST);
						meta->pxVehicle->mDriveDynData.setUseAutoGears(false);

						if (player_is_left)
							meta->pxVehicleInputData.setAnalogSteer(-1);
						else
							meta->pxVehicleInputData.setAnalogSteer(1);

					}
					else { // driving towards player

						meta->pxVehicle->mDriveDynData.setUseAutoGears(true);

						const auto pointed_at_player =
							-0.3f <= player_direction && player_direction <= 0.3f;

						if (pointed_at_player) { /* drive straight */
						}
						else {                 // steer towards player
							if (player_is_left)
								meta->pxVehicleInputData.setAnalogSteer(-1);
							else
								meta->pxVehicleInputData.setAnalogSteer(1);
						}
					}
				}
				else {
					auto player_direction = perpdot(
						glm::normalize(player_location - ai_location), ai_direction);
					// check if the player is in front or behind the ai.
					auto heading =
						[p = glm::normalize(glm::vec3(player_location - ai_location)),
						b = glm::vec3(ai_direction)]() { return glm::dot(p, b); };

					const auto ai_is_driving_away = heading() < 0.0f;
					const auto player_is_left = player_direction > 0;

					if (ai_is_driving_away) { // turn the ai around...

						meta->pxVehicle->mDriveDynData.forceGearChange(
							PxVehicleGearsData::eFIRST);
						meta->pxVehicle->mDriveDynData.setUseAutoGears(false);

						if (player_is_left)
							meta->pxVehicleInputData.setAnalogSteer(-1);
						else
							meta->pxVehicleInputData.setAnalogSteer(1);

					}
					else { // driving towards player

						meta->pxVehicle->mDriveDynData.setUseAutoGears(true);

						const auto pointed_at_player =
							-0.3f <= player_direction && player_direction <= 0.3f;

						if (pointed_at_player) { /* drive straight */
						}
						else {                 // steer towards player
							if (player_is_left)
								meta->pxVehicleInputData.setAnalogSteer(-1);
							else
								meta->pxVehicleInputData.setAnalogSteer(1);
						}
					}
				}
	};
	std::shared_ptr<EventHandler<Vehicle*>> ticker =
		getEngine()->getSubSystem<EventSystem>()->createHandler(ai_tick_callback);

	//
	nodeType** navEnum = new nodeType * [64];
	for (int i = 0; i < 64; i++) {
		navEnum[i] = new nodeType[64];
	}
	for (int i = 0; i < 64; i++)
		for (int j = 0; j < 64; j++) {
			navEnum[i][j] = static_cast<nodeType>(nav[64 * j + i]);
		}

	// spawn some ai bois into the world
	auto dim = 1;
	int spacing = 90;
	for (int x = -dim; x <= dim; x++) {
		for (int y = -dim; y <= dim; y++) {
			auto ai_vehicle =
				make_ai(glm::translate(glm::vec3(x * spacing, 30, y * spacing + 10)));
			ai_vehicle->local_rotation = glm::rotate(3.14159f, glm::vec3(0, 1, 0));
			ai_vehicle->path.graphNodes = navEnum;
			ai_vehicle->path.FindPath(player_vehicle->position, ai_vehicle->position);
			ai_vehicle->path.CleanPath();
			// ai_vehicle->path.PrintPath();
			ai_vehicle->tickHandler += ticker; // give them brain
		}
	}

	// make a default camera
	// auto camera =  getEngine()->createComponent<Component::Camera>();
	// camera->target = player_vehicle; // make camera follow player.
	// inputSystem->onGamePadStateChanged +=
	// cameraSystem->onGamepadStateChangedHandler;

	// region initialize game-clocks
	using namespace std::chrono;
	typedef duration<float> floatMilliseconds; // define this to get float values;
	auto start = high_resolution_clock::now();
	auto end = start + milliseconds(1); // do this so physx doesn't complain about
										// time being 0. endregion
}

TestWorld::TestWorld(EngineSystem* enginePtr) : ScenarioInterface(enginePtr) {}
