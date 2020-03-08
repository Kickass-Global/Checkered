
#include "testworld.hpp"
#include <functional>

#include "Pipeline/EntityLoader.h"
#include "Damage/damagesystem.hpp"
#include <PxPhysicsAPI.h>
#include <Vehicle/vehiclesystem.hpp>
#include <Vehicle.h>
#include "Physics/PhysicsSystem.h"
#include "Camera/CameraSystem.h"
#include "Input/InputSystem.h"
#include "Debug/LiveReloadSystem.h"
#include "Passenger.h"

#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include "Component/scenecomponentsystem.hpp"
#include "Navigation/astar.h"
#include "systeminterface.hpp"
#include "Engine.h"
#include "WorldTransform.h"
#include <Events/Events.h>
#include "soundSystem/SoundSystem.h"
#include <texture.hpp>
#include <fstream>
#include <material.hpp>
#include <Billboard.h>
#include "scenery.hpp"

void TestWorld::load() {
	auto physicsSystem = Engine::addSystem<Physics::PhysicsSystem>();
	auto vehicleSystem = Engine::addSystem<Engine::vehicleSystem>();
	auto openALSoundSystem = Engine::addSystem<Engine::SoundSystem>();

	vehicleSystem->onVehicleCreated += physicsSystem->onVehicleCreatedHandler;

	auto componentSystem = Engine::addSystem<Component::SceneComponentSystem>();
	componentSystem->onActorCreated += physicsSystem->onActorCreatedHandler;
	Engine::addSystem<Engine::DamageSystem>();
	auto cameraSystem = Engine::addSystem<::Camera::CameraSystem>();
	auto renderingSystem = Engine::addSystem<Rendering::RenderingSystem>();
	auto liveReloadSystem = Engine::addSystem<Debug::LiveReloadSystem>();
	auto inputSystem = Engine::addSystem<Input::InputSystem>();

	auto hornSystem = Engine::addSystem<Horn::hornSystem>();
	// hookup inputs from current window
	inputSystem->initialize(renderingSystem->getWindow());

	// hookup key press event with camera system
	auto eventSystem = Engine::addSystem<Engine::EventSystem>();
	eventSystem->order = 0;
	renderingSystem->order = 2;

	Input::InputSystem::onKeyPress += cameraSystem->onKeyPressHandler;
	Input::InputSystem::onKeyDown += cameraSystem->onKeyDownHandler;
	Input::InputSystem::onKeyUp += cameraSystem->onKeyUpHandler;

	Input::InputSystem::onKeyPress += physicsSystem->onKeyPressHandler;
	Input::InputSystem::onKeyDown += physicsSystem->onKeyDownHandler;
	Input::InputSystem::onKeyUp += physicsSystem->onKeyUpHandler;

	Input::InputSystem::onKeyPress += hornSystem->onKeyPressHandler;
	Input::InputSystem::onKeyDown += hornSystem->onKeyDownHandler;
	Input::InputSystem::onKeyUp += hornSystem->onKeyUpHandler;

	Rendering::RenderingSystem::onWindowSizeChanged += cameraSystem->onWindowSizeHandler;
	//endregion

	// setup the ground mesh

	auto basic_shader_program = Pipeline::Library::getAsset<Program>(
		"Assets/Programs/basic.json"
		);


	// create a scene object to hold the ground components to follow.
	Instance<DrivableScenery> drivable_instances;

	// make a material component
	auto ground_material = Engine::createComponent<Component::Material>(basic_shader_program);
	ground_material->textures.push_back(Engine::createComponent<Component::Texture>("Assets/Textures/checkeredMap2.jpg"));
	ground_material->shader = Pipeline::Library::getAsset<Program>(
		"Assets/Programs/basic.json"
		);

	// load the mesh component
	auto plane_mesh = Pipeline::Library::getAsset<Mesh>("Assets/Meshes/checkeredMap2.fbx");
	drivable_instances.add_instance_at(
		glm::rotate(glm::radians(-90.0f), glm::vec3{ 1,0,0 }) *glm::translate(glm::vec3{ 0,-1,0 }),
		plane_mesh, ground_material, plane_mesh
	);


	// create some buildings

	auto building_material1 = Engine::createComponent<Component::Material>(basic_shader_program);
	auto building_material2 = Engine::createComponent<Component::Material>(basic_shader_program);
	auto building_material3 = Engine::createComponent<Component::Material>(basic_shader_program);


	building_material1->textures.push_back(
		Engine::createComponent<Component::Texture>("Assets/Textures/House_01.png")
	);
	building_material2->textures.push_back(
		Engine::createComponent<Component::Texture>("Assets/Textures/House_02.png")
	);
	building_material3->textures.push_back(
		Engine::createComponent<Component::Texture>("Assets/Textures/House_03.png")
	);

	auto building_mesh1 = Pipeline::Library::getAsset<Mesh>(
		"Assets/Meshes/Building_House_01.fbx"
		);
	auto building_mesh2 = Pipeline::Library::getAsset<Mesh>(
		"Assets/Meshes/Building_House_013.fbx"
		);
	auto building_mesh3 = Pipeline::Library::getAsset<Mesh>(
		"Assets/Meshes/Building_Shop_02.fbx"
		);

	Instance<Scenery> building_instances;
	building_instances.add_instance_at(glm::vec3{ 43,0,-60 }, building_mesh1, building_material1);
	building_instances.add_instance_at(glm::vec3{ 10,0,-83 }, building_mesh2, building_material2);
	building_instances.add_instance_at(glm::vec3{ -20,0,-86 }, building_mesh3, building_material3);
	building_instances.add_instance_at(glm::vec3{ 0,0,-80 }, building_mesh3, building_material3);


	// setup a HUD element...

	auto billboard_mesh = Pipeline::Library::getAsset<Mesh>("Assets/Meshes/billboard_quad.obj");
	billboard_mesh->emplaceChildComponent<WorldTransform>();


	auto sprite = Engine::createComponent<Component::Billboard>();
	sprite->plot = { 10, 10, 100, 100 };
	{
		auto material = Engine::createComponent<Component::Material>(
			Pipeline::Library::getAsset<Program>(
				"Assets/Programs/billboard.json"
				)
			);

		material->textures.push_back(
			Engine::createComponent<Component::Texture>("Assets/Textures/Nature_Trees.png"));

		sprite->mesh_instance = Engine::createNamedComponent<MeshInstance>(
			"billboard_mesh_instance",
			billboard_mesh,
			material
			);
	}

	// setup the mesh used for the cars...

	auto car_material = Engine::createComponent<Component::Material>(basic_shader_program);
	car_material->textures.push_back(
		Engine::createComponent<Component::Texture>("Assets/Textures/Vehicle_Car01_c.png"));
	auto car_mesh = Pipeline::Library::getAsset<Mesh>(
		"Assets/Meshes/cop_mesh.fbx"
		);

	auto car_mesh_instance = Engine::createNamedComponent<MeshInstance>("car_mesh_instance",
		car_mesh, car_material);

	auto taxi_material = Engine::createComponent<Component::Material>(basic_shader_program);
	taxi_material->textures.push_back(
		Engine::createComponent<Component::Texture>("Assets/Textures/Vehicle_Taxi.png"));

	auto taxi_mesh = Pipeline::Library::getAsset<Mesh>(
		"Assets/Meshes/taxi_mesh.fbx"
		);


	// setup the vehicle for the player...

	auto player = Engine::createNamedComponent<Component::ControlledVehicle>("player");
	inputSystem->onGamePadStateChanged += player->onGamePadStateChangedHandler;
	inputSystem->onKeyUp += player->onKeyUpHandler;
	inputSystem->onKeyDown += player->onKeyDownHandler;

	auto &player_vehicle = player->vehicle;

	auto player_damage_model = Engine::createNamedComponent<Component::Model>("player_damage_model");

	player_damage_model->parts.push_back(Component::Model::Part{});
	player_damage_model->parts[0].variations.push_back(Component::Model::Variation{ 2000000, Engine::createComponent<PaintedMesh>(car_mesh, car_material) });

	player_vehicle->model = player_damage_model;
	player_vehicle->local_rotation = glm::rotate(3.14159f, glm::vec3(0, 1, 0));
	player_vehicle->local_position = glm::vec3(0.0f, -1.0f, 0.0f);
	player_vehicle->position = glm::vec3(0.0f, 0.0f, -40.0f);
	physicsSystem->playerVehicle = player_vehicle;

	//setup passenger system

	Instance<Obstacle> obstacle_instances;
	auto dumpster_material = Engine::createComponent<Component::Material>(basic_shader_program);

	dumpster_material->textures.push_back(
		Engine::createComponent<Component::Texture>("Assets/Textures/Road_divider.png")
	);
	auto dumpster_mesh = Pipeline::Library::getAsset<Mesh>(
		"Assets/Meshes/dumpster_mesh.fbx"
		);

	auto hydrant_material = Engine::createComponent<Component::Material>(basic_shader_program);
	hydrant_material->textures.push_back(
		Engine::createComponent<Component::Texture>("Assets/Textures/Props.png")
	);
	auto hydrant_mesh = Pipeline::Library::getAsset<Mesh>(
		"Assets/Meshes/Props_Buoy_02.fbx"
		);	
	

	auto tree_material = Engine::createComponent<Component::Material>(basic_shader_program);
	tree_material->textures.push_back(
		Engine::createComponent<Component::Texture>("Assets/Textures/Nature_Trees.png")
	);

	auto tree_mesh = Pipeline::Library::getAsset<Mesh>(
			"Assets/Meshes/Prop_Tree_02.fbx"
		);

	glm::vec3 origin = { 10,0,20 };
	for (int i = 1; i < 50; i++) {
		obstacle_instances.add_instance_at(i / 50.0f * 200.0f * glm::vec3{ std::cos(i), std::acos(i / 200.0), std::sin(i) } -glm::vec3{ 0,5,0 } +origin, hydrant_mesh, hydrant_material);
	}

	auto passenger = Engine::createComponent<Component::Passenger>(
		glm::vec3{ 0.0f, 0.0f, -20.0f },
		glm::vec3{ 0.0f, 0.0f, 20.0f },
		tree_mesh,
		tree_material
		);

	passenger->onPassengerDroppedOffDelegate += [](int id) {

		auto billboard_mesh = Pipeline::Library::getAsset<Mesh>("Assets/Meshes/billboard_quad.obj");
		billboard_mesh->emplaceChildComponent<WorldTransform>();


		auto sprite = Engine::createComponent<Component::Billboard>();
		sprite->plot = { 10, 10, 630, 470 };
		{
			auto material = Engine::createComponent<Component::Material>(
				Pipeline::Library::getAsset<Program>(
					"Assets/Programs/billboard.json"
					)
				);

			material->textures.push_back(
				Engine::createComponent<Component::Texture>("Assets/Textures/win.jpg"));

			sprite->mesh_instance = Engine::createNamedComponent<MeshInstance>(
				"billboard_mesh_instance",
				billboard_mesh,
				material
				);
		}
	};

	using namespace Engine;

	// ai factory method...
	auto make_ai = [&taxi_mesh, &taxi_material](glm::mat4 world_transform = glm::mat4{ 1 }) {

		auto ai_vehicle = Engine::createNamedComponent<Component::Vehicle>("ai_vehicle");
		auto ai_damage_model = Engine::createNamedComponent<Component::Model>("ai_vehicle_model");

		ai_damage_model->parts.push_back(Component::Model::Part{});
		ai_damage_model->parts[0].variations.push_back(Component::Model::Variation{ 2000000, Engine::createComponent<PaintedMesh>(taxi_mesh, taxi_material) });

		glm::quat orientation;
		glm::vec3 scale, translation, skew;
		glm::vec4 perspective;
		glm::decompose(world_transform, scale, orientation, translation, skew, perspective);

		ai_vehicle->model = ai_damage_model;
		ai_vehicle->rotation = orientation;
		ai_vehicle->position = translation;
		ai_vehicle->local_position = glm::vec3(0.0f, -1.45f, 0.0f);

		return ai_vehicle;
	};


	// setup ai "brain"
	std::function<void(const Component::EventArgs<Vehicle*>&)> ai_tick_callback = [player_vehicle](
		const Component::EventArgs<Vehicle*>& args) {
		auto meta = std::get<0>(args.values);

		auto player_location = player_vehicle->position; // translation vector of mat4
		auto ai_direction = glm::normalize(-meta->world_transform()[2]); // 'z' column vector of mat4 (i.e. forward)
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
			auto player_direction = perpdot(glm::normalize(
				glm::vec3(nextPoint->my_x + 1.5f, 0, nextPoint->my_z + 1.5f) - ai_location), ai_direction);
			// check if the point is in front or behind the ai.
			auto heading = [p = glm::normalize(
				glm::vec3(nextPoint->my_x + 1.5f, 0, nextPoint->my_z + 1.5f) - ai_location), b = glm::vec3(ai_direction)]() {
				return glm::dot(p, b);
			};

			const auto ai_is_driving_away = heading() < 0.0f;
			const auto player_is_left = player_direction > 0;

			if (ai_is_driving_away) { // turn the ai around...

				meta->pxVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
				meta->pxVehicle->mDriveDynData.setUseAutoGears(false);

				if (player_is_left) meta->pxVehicleInputData.setAnalogSteer(-1);
				else meta->pxVehicleInputData.setAnalogSteer(1);

			}
			else { // driving towards player

				meta->pxVehicle->mDriveDynData.setUseAutoGears(true);

				const auto pointed_at_player = -0.3f <= player_direction && player_direction <= 0.3f;

				if (pointed_at_player) { /* drive straight */ }
				else { // steer towards player
					if (player_is_left) meta->pxVehicleInputData.setAnalogSteer(-1);
					else meta->pxVehicleInputData.setAnalogSteer(1);
				}

			}
		}
		else {
			auto player_direction = perpdot(glm::normalize(player_location - ai_location), ai_direction);
			// check if the player is in front or behind the ai.
			auto heading = [p = glm::normalize(glm::vec3(player_location - ai_location)), b = glm::vec3(ai_direction)]() {
				return glm::dot(p, b);
			};

			const auto ai_is_driving_away = heading() < 0.0f;
			const auto player_is_left = player_direction > 0;

			if (ai_is_driving_away) { // turn the ai around...

				meta->pxVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
				meta->pxVehicle->mDriveDynData.setUseAutoGears(false);

				if (player_is_left) meta->pxVehicleInputData.setAnalogSteer(-1);
				else meta->pxVehicleInputData.setAnalogSteer(1);

			}
			else { // driving towards player

				meta->pxVehicle->mDriveDynData.setUseAutoGears(true);

				const auto pointed_at_player = -0.3f <= player_direction && player_direction <= 0.3f;

				if (pointed_at_player) { /* drive straight */ }
				else { // steer towards player
					if (player_is_left) meta->pxVehicleInputData.setAnalogSteer(-1);
					else meta->pxVehicleInputData.setAnalogSteer(1);
				}

			}
		}



	};
	std::shared_ptr<EventHandler<Vehicle*>> ticker = Engine::EventSystem::createHandler(ai_tick_callback);

	//
	nodeType** navEnum = new nodeType *[64];
	for (int i = 0; i < 64; i++) { navEnum[i] = new nodeType[64]; }
	for (int i = 0; i < 64; i++) for (int j = 0; j < 64; j++) { navEnum[i][j] = static_cast<nodeType>(nav[64 * j + i]); }

	// spawn some ai bois into the world
	auto dim = 1;
	int spacing = 120;
	for (int x = -dim; x <= dim; x++) {
		for (int y = -dim; y <= dim; y++) {
			auto ai_vehicle = make_ai(glm::translate(glm::vec3(x * spacing, 0, y * spacing + 10)));
			ai_vehicle->local_rotation = glm::rotate(3.14159f, glm::vec3(0, 1, 0));
			ai_vehicle->path.graphNodes = navEnum;
			ai_vehicle->path.FindPath(player_vehicle->position, ai_vehicle->position);
			ai_vehicle->path.CleanPath();
			ai_vehicle->path.PrintPath();
			ai_vehicle->tickHandler += ticker; // give them brain

		}
	}

	// make a default camera
	auto camera = Engine::createComponent<Component::Camera>();
	camera->target = player_vehicle; // make camera follow player.
	inputSystem->onGamePadStateChanged += cameraSystem->onGamepadStateChangedHandler;

	// region initialize game-clocks
	using namespace std::chrono;
	typedef duration<float> floatMilliseconds; // define this to get float values;
	auto start = high_resolution_clock::now();
	auto end = start + milliseconds(1); // do this so physx doesn't complain about time being 0.
	// endregion
}