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

#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include "Systems/Component/scenecomponentsystem.hpp"
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

int main() {

	// region initialize engine systems
	using namespace Engine;

	auto running = true;

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

	//Input::InputSystem::onKeyPress += hornSystem->onKeyPressHandler;
	//Input::InputSystem::onKeyDown += hornSystem->onKeyDownHandler;
	//Input::InputSystem::onKeyUp += hornSystem->onKeyUpHandler;

	Rendering::RenderingSystem::onWindowSizeChanged += cameraSystem->onWindowSizeHandler;
	//endregion

	// setup the ground mesh

	auto basic_shader_program = Pipeline::Library::getAsset<Program>(
		"Assets/Programs/basic.json"
		);


	// create a scene object to hold the ground components to follow.
	auto ground_object = Engine::createComponent<Component::SceneComponent>(nullptr);

	// make a material component
	auto ground_material = Engine::createComponent<Component::Material>(basic_shader_program);
	ground_material->textures.push_back(Engine::createComponent<Component::Texture>("Assets/Textures/white.png"));
	ground_material->shader = Pipeline::Library::getAsset<Program>(
		"Assets/Programs/basic.json"
		);

	// load the mesh component
	auto plane_mesh = Pipeline::Library::getAsset<Mesh>("Assets/Meshes/plane.obj");

	//    // create a mesh isnatnce (binds material with mesh)
	auto ground_mesh_instance = Engine::createNamedComponent<Component::MeshInstance>(
		"ground_mesh_instance", plane_mesh, ground_material
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
	building_instances.add_instance_at({ 43,0,-10 }, building_mesh1, building_material1);
	building_instances.add_instance_at({ 10,0,-23 }, building_mesh2, building_material2);
	building_instances.add_instance_at({ -20,0,-16 }, building_mesh3, building_material3);


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

	auto car_mesh_instance = Engine::createNamedComponent<MeshInstance>("car_mesh_instance",
		Pipeline::Library::getAsset<Mesh>(
			"Assets/Meshes/car_mesh.fbx"
			), car_material);

	auto taxi_material = Engine::createComponent<Component::Material>(basic_shader_program);
	taxi_material->textures.push_back(
		Engine::createComponent<Component::Texture>("Assets/Textures/Vehicle_Taxi.png"));

	auto taxi_mesh_instance = Engine::createNamedComponent<MeshInstance>("car_mesh_instance",
		Pipeline::Library::getAsset<Mesh>(
			"Assets/Meshes/taxi_mesh.fbx"
			), taxi_material);

	// setup the vehicle for the player...

	auto player_vehicle = Engine::createNamedComponent<Component::Vehicle>("player_vehicle");
	auto player_damage_model = Engine::createNamedComponent<Component::Model>("player_damage_model");

	player_damage_model->parts.push_back(Component::Model::Part{});
	player_damage_model->parts[0].variations.push_back(Component::Model::Variation{ 2000000, car_mesh_instance });

	player_vehicle->model = player_damage_model;
	player_vehicle->local_rotation = glm::rotate(3.14159f, glm::vec3(0, 1, 0));
	player_vehicle->local_position = glm::vec3(0.0f, -1.0f, 0.0f);
	player_vehicle->position = glm::vec3(0.0f, 0.0f, -40.0f);
	//setup passenger system
	PxTransform temp_pickup(0.f, 0.f, 2.0f);
	PxTransform temp_dropoff(3.0f, 0.f, 2.0f);
	auto passenger = Engine::createComponent<Component::Passenger>();
	passenger->setPickupTransform(temp_pickup);
	passenger->setDropoffTransform(temp_dropoff);
	physicsSystem->onPassengerCreated(passenger);
	physicsSystem->playerVehicle = player_vehicle;

	// ai factory method...
	auto make_ai = [taxi_mesh_instance](glm::mat4 world_transform = glm::mat4{ 1 }) {

		auto ai_vehicle = Engine::createNamedComponent<Component::Vehicle>("ai_vehicle");
		auto ai_damage_model = Engine::createNamedComponent<Component::Model>("ai_vehicle_model");

		ai_damage_model->parts.push_back(Component::Model::Part{});
		ai_damage_model->parts[0].variations.push_back(Component::Model::Variation{ 2000000, taxi_mesh_instance });

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

	//setup passenger
	//auto passenger_entity = Engine::createComponent<Component::Passenger>();
	//passenger_entity->initPassenger();

	// setup ai "brain"

	std::function<void(const Component::EventArgs<Vehicle*>&)> ai_tick_callback =
		[player_vehicle](const Component::EventArgs<Vehicle*>& args) {
		auto meta = std::get<0>(args.values);

		auto player_location = glm::normalize(player_vehicle->position); // translation vector of mat4
		auto ai_direction = glm::normalize(-meta->world_transform()[2]); // 'z' column vector of mat4 (i.e. forward)
		auto ai_location = glm::normalize(meta->position); // translation vector of mat4

		// check if the player is to the left or right of the ai.
		auto perpdot = [](auto v1, auto v2) {
			return v1.z * v2.x - v1.x * v2.z;
		};

		// check if the player is in front or behind the ai.
		auto heading = [p = glm::normalize(glm::vec3(player_location - ai_location)), b = glm::vec3(ai_direction)]() {
			return glm::dot(p, b);
		};

		meta->pxVehicleInputData.setDigitalSteerLeft(false);
		meta->pxVehicleInputData.setDigitalSteerRight(false);

		auto player_direction = perpdot(player_location - ai_location, ai_direction);

		// todo control throttle better...
		meta->pxVehicleInputData.setDigitalAccel(true);

		const auto ai_is_driving_away = heading() < 0.0f;
		const auto player_is_left = player_direction > 0;

		if (ai_is_driving_away) { // turn the ai around...

			meta->pxVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
			meta->pxVehicle->mDriveDynData.setUseAutoGears(false);

			if (player_is_left) meta->pxVehicleInputData.setDigitalSteerLeft(true);
			else meta->pxVehicleInputData.setDigitalSteerRight(true);

		}
		else { // driving towards player

			meta->pxVehicle->mDriveDynData.setUseAutoGears(true);

			const auto pointed_at_player = -0.3f <= player_direction && player_direction <= 0.3f;

			if (pointed_at_player) { /* drive straight */ }
			else { // steer towards player
				if (player_is_left) meta->pxVehicleInputData.setDigitalSteerLeft(true);
				else meta->pxVehicleInputData.setDigitalSteerRight(true);
			}

		}
	};

	std::shared_ptr<EventHandler<Vehicle*>> ticker = Engine::EventSystem::createHandler(ai_tick_callback);

	// spawn some ai bois into the world
	auto dim = 1;
	int spacing = 123;
	for (int x = -dim; x <= dim; x++) {
		for (int y = -dim; y <= dim; y++) {

			auto ai_vehicle = make_ai(glm::translate(glm::vec3(x * spacing, 0, y * spacing)));
			ai_vehicle->local_rotation = glm::rotate(3.14159f, glm::vec3(0, 1, 0));
			ai_vehicle->tickHandler += ticker; // give them brain

		}
	}

	// make a default camera
	auto camera = Engine::createComponent<Component::Camera>();
	camera->target = player_vehicle; // make camera follow player.

	// region initialize game-clocks
	using namespace std::chrono;
	typedef duration<float> floatMilliseconds; // define this to get float values;
	auto start = high_resolution_clock::now();
	auto end = start + milliseconds(1); // do this so physx doesn't complain about time being 0.
	// endregion

	Engine::sortSystems();

	while (running) {

		// region before update
		floatMilliseconds delta = end - start;
		Engine::deltaTime elapsed = static_cast<Engine::deltaTime>(duration_cast<milliseconds>(delta).count());
		// endregion		

		Engine::EventSystem::onTick(elapsed);

		// todo remove these hacks...
		// force the ground to render...

		for (const auto& system : Engine::systems()) {
			system->update(elapsed);
		}

		// region after update
		start = end;
		end = high_resolution_clock::now();
		//endregion
	}
}
