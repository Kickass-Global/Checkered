//
// Created by Jackson Cougar Wiebe on 4/3/2020.
//

#ifndef ENGINE_PLAYERVEHICLE_HPP
#define ENGINE_PLAYERVEHICLE_HPP

#include "Billboard.h"
#include "Vehicle.h"
#include <ComponentBase.h>
#include <Engine.h>
#include <HealthBar.hpp>
#include <Input/InputSystem.h>
#include <Pipeline/Library.h>
#include <Waypoint.hpp>

namespace Component {

  struct Player : public ComponentBase {

    std::shared_ptr<ControlledVehicle> controller;
    Player() {

      controller = getEngine()->createNamedComponent<Component::ControlledVehicle>("player");
      auto inputSystem = getEngine()->getSubSystem<Input::InputSystem>();
      inputSystem->onGamePadStateChanged += controller->onGamePadStateChangedHandler;
      inputSystem->onKeyUp += controller->onKeyUpHandler;
      inputSystem->onKeyDown += controller->onKeyDownHandler;

      auto car_material = getEngine()->createComponent<Component::Material>();
      auto liscense_material = getEngine()->createComponent<Component::Material>();

      car_material->textures.push_back(
          getEngine()->createComponent<Component::Texture>("Assets/Textures/Vehicle_Car01_c.png"));

      liscense_material->textures.push_back(
          getEngine()->createComponent<Component::Texture>("Assets/Textures/liscense.png"));

      auto car_mesh = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
          "Assets/Meshes/car_body.obj");
      auto car_left_mirror = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
          "Assets/Meshes/car_left_mirror.obj");
      auto car_right_mirror = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
          "Assets/Meshes/car_right_mirror.obj");
      auto car_front_bumper = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
          "Assets/Meshes/car_front_bumper.obj");
      auto car_back_bumper = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
          "Assets/Meshes/car_back_bumper.obj");
      auto car_liscense = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
          "Assets/Meshes/car_liscense.obj");
      auto car_tire =
          getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>("Assets/Meshes/wheel.obj");

      car_mesh->cast_shadow = true;

      auto &player_vehicle = controller->vehicle;

      auto player_damage_model = player_vehicle->model;

      player_damage_model->parts.push_back(Component::Model::Part{{}, 0, "car_tire", 0, 500});
      player_damage_model->parts.push_back(Component::Model::Part{{}, 0, "car_tire", 0, 500});
      player_damage_model->parts.push_back(Component::Model::Part{{}, 0, "car_tire", 0, 500});
      player_damage_model->parts.push_back(Component::Model::Part{{}, 0, "car_tire", 0, 500});

      player_damage_model->parts.push_back(Component::Model::Part{{}, 0, "chassis", 0, 100, true});
      player_damage_model->parts.push_back(Component::Model::Part{{}, 0, "car_left_mirror", 0, 5});
      player_damage_model->parts.push_back(Component::Model::Part{{}, 0, "car_right_mirror", 0, 5});
      player_damage_model->parts.push_back(
          Component::Model::Part{{}, 0, "car_front_bumper", 0, 40});
      player_damage_model->parts.push_back(Component::Model::Part{{}, 0, "car_back_bumper", 0, 40});
      player_damage_model->parts.push_back(Component::Model::Part{{}, 0, "car_liscense", 0, 5});


      player_damage_model->parts[0].transform = glm::translate(glm::vec3{1.117f, 0.3635f, -1.6223});
      player_damage_model->parts[1].transform = glm::translate(glm::vec3{-1.117f, 0.3635f, 1.7342});
      player_damage_model->parts[2].transform = glm::translate(glm::vec3{1.117f, 0.3635f, -1.6223});
      player_damage_model->parts[3].transform = glm::translate(glm::vec3{-1.117f, 0.3635f, 1.7342});
      player_damage_model->parts[0].is_wheel = true;
      player_damage_model->parts[1].is_wheel = true;
      player_damage_model->parts[2].is_wheel = true;
      player_damage_model->parts[3].is_wheel = true;

      player_damage_model->parts[0].variations.push_back(Component::Model::Variation{
          500, getEngine()->createComponent<PaintedMesh>(car_tire, car_material)});
      player_damage_model->parts[1].variations.push_back(Component::Model::Variation{
          500, getEngine()->createComponent<PaintedMesh>(car_tire, car_material)});
      player_damage_model->parts[2].variations.push_back(Component::Model::Variation{
          500, getEngine()->createComponent<PaintedMesh>(car_tire, car_material)});
      player_damage_model->parts[3].variations.push_back(Component::Model::Variation{
          500, getEngine()->createComponent<PaintedMesh>(car_tire, car_material)});

      size_t chassis = 4;
      player_damage_model->parts[chassis + 0].variations.push_back(Component::Model::Variation{
          100, getEngine()->createComponent<PaintedMesh>(car_mesh, car_material)});

      player_damage_model->parts[chassis + 1].variations.push_back(Component::Model::Variation{
          5, getEngine()->createComponent<PaintedMesh>(car_left_mirror, car_material)});

      player_damage_model->parts[chassis + 2].variations.push_back(Component::Model::Variation{
          5, getEngine()->createComponent<PaintedMesh>(car_right_mirror, car_material)});

      player_damage_model->parts[chassis + 3].variations.push_back(Component::Model::Variation{
          40, getEngine()->createComponent<PaintedMesh>(car_front_bumper, car_material)});

      player_damage_model->parts[chassis + 4].variations.push_back(Component::Model::Variation{
          40, getEngine()->createComponent<PaintedMesh>(car_back_bumper, car_material)});

      player_damage_model->parts[chassis + 5].variations.push_back(Component::Model::Variation{
          40, getEngine()->createComponent<PaintedMesh>(car_liscense, liscense_material)});


      player_damage_model->onHealthChanged += [this](auto health) {
        if (health < 1) {
          log<high>("He's dead Jim");
          /*auto matte = getEngine()->createComponent<Billboard>(
              getEngine()->createComponent<Texture>("Assets/Textures/checkered-matte.png"));
          matte->dst = {0, 0};
          matte->src = {0, 0};*/
        }
      };

      // when a region is destroyed we want to detach the shape from the vehicle
      // and spawn a new physics actors object into the game at its location.

      player_vehicle->local_rotation = glm::rotate(3.14159f, glm::vec3(0, 1, 0));
      player_vehicle->local_position = glm::vec3(0.0f, -2.0f, 0.0f);
      player_vehicle->position = glm::vec3(0.0f, 3.0f, -40.0f);

      player_vehicle->front_right_wheel = &player_damage_model->parts[6];
      player_vehicle->front_left_wheel = &player_damage_model->parts[7];
      player_vehicle->back_right_wheel = &player_damage_model->parts[8];
      player_vehicle->back_left_wheel = &player_damage_model->parts[9];

      auto arrow = getEngine()->createComponent<WaypointArrow>();
      arrow->target_vehicle = player_vehicle;

      auto health_bar = getEngine()->createComponent<HealthBar>();
      health_bar->target = player_damage_model;

      getEngine()->getSubSystem<EventSystem>()->onTick +=
          std::bind(&WaypointArrow::tick, arrow, std::placeholders::_1);

      getEngine()->getSubSystem<EventSystem>()->onTick +=
          std::bind(&HealthBar::tick, health_bar, std::placeholders::_1);
    }
  };
  struct Taxi : public ComponentBase {

      std::shared_ptr<Vehicle> player_vehicle;

    Taxi() {


      auto car_material = getEngine()->createComponent<Component::Material>();
      auto liscense_material = getEngine()->createComponent<Component::Material>();

      car_material->textures.push_back(
          getEngine()->createComponent<Component::Texture>("Assets/Textures/Vehicle_Taxi.png"));

      liscense_material->textures.push_back(
          getEngine()->createComponent<Component::Texture>("Assets/Textures/liscense.png"));

      auto car_mesh = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
          "Assets/Meshes/taxi_mesh.fbx");
      auto car_left_mirror = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
          "Assets/Meshes/car_left_mirror.obj");
      auto car_right_mirror = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
          "Assets/Meshes/car_right_mirror.obj");
      auto car_front_bumper = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
          "Assets/Meshes/car_front_bumper.obj");
      auto car_back_bumper = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
          "Assets/Meshes/car_back_bumper.obj");
      auto car_liscense = getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
          "Assets/Meshes/car_liscense.obj");
      auto car_tire =
          getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>("Assets/Meshes/wheel.obj");

      car_mesh->cast_shadow = true;

      player_vehicle = getEngine()->createComponent<Vehicle>();
      auto player_damage_model = player_vehicle->model;

      player_damage_model->parts.push_back(Component::Model::Part{{}, 0, "car_tire", 0, 500});
      player_damage_model->parts.push_back(Component::Model::Part{{}, 0, "car_tire", 0, 500});
      player_damage_model->parts.push_back(Component::Model::Part{{}, 0, "car_tire", 0, 500});
      player_damage_model->parts.push_back(Component::Model::Part{{}, 0, "car_tire", 0, 500});

      player_damage_model->parts.push_back(Component::Model::Part{{}, 0, "chassis", 0, 10, true});
      player_damage_model->parts.push_back(Component::Model::Part{{}, 0, "car_left_mirror", 0, 5});
      player_damage_model->parts.push_back(Component::Model::Part{{}, 0, "car_right_mirror", 0, 5});
      player_damage_model->parts.push_back(
          Component::Model::Part{{}, 0, "car_front_bumper", 0, 40});
      player_damage_model->parts.push_back(Component::Model::Part{{}, 0, "car_back_bumper", 0, 40});
      player_damage_model->parts.push_back(Component::Model::Part{{}, 0, "car_liscense", 0, 5});


      player_damage_model->parts[0].transform = glm::translate(glm::vec3{1.117f, 0.3635f, -1.6223});
      player_damage_model->parts[1].transform = glm::translate(glm::vec3{-1.117f, 0.3635f, 1.7342});
      player_damage_model->parts[2].transform = glm::translate(glm::vec3{1.117f, 0.3635f, -1.6223});
      player_damage_model->parts[3].transform = glm::translate(glm::vec3{-1.117f, 0.3635f, 1.7342});
      player_damage_model->parts[0].is_wheel = true;
      player_damage_model->parts[1].is_wheel = true;
      player_damage_model->parts[2].is_wheel = true;
      player_damage_model->parts[3].is_wheel = true;

      player_damage_model->parts[0].variations.push_back(Component::Model::Variation{
          500, getEngine()->createComponent<PaintedMesh>(car_tire, car_material)});
      player_damage_model->parts[1].variations.push_back(Component::Model::Variation{
          500, getEngine()->createComponent<PaintedMesh>(car_tire, car_material)});
      player_damage_model->parts[2].variations.push_back(Component::Model::Variation{
          500, getEngine()->createComponent<PaintedMesh>(car_tire, car_material)});
      player_damage_model->parts[3].variations.push_back(Component::Model::Variation{
          500, getEngine()->createComponent<PaintedMesh>(car_tire, car_material)});

      size_t chassis = 4;
      player_damage_model->parts[chassis + 0].variations.push_back(Component::Model::Variation{
          10, getEngine()->createComponent<PaintedMesh>(car_mesh, car_material)});

      player_damage_model->parts[chassis + 1].variations.push_back(Component::Model::Variation{
          5, getEngine()->createComponent<PaintedMesh>(car_left_mirror, car_material)});

      player_damage_model->parts[chassis + 2].variations.push_back(Component::Model::Variation{
          5, getEngine()->createComponent<PaintedMesh>(car_right_mirror, car_material)});

      player_damage_model->parts[chassis + 3].variations.push_back(Component::Model::Variation{
          40, getEngine()->createComponent<PaintedMesh>(car_front_bumper, car_material)});

      player_damage_model->parts[chassis + 4].variations.push_back(Component::Model::Variation{
          40, getEngine()->createComponent<PaintedMesh>(car_back_bumper, car_material)});

      player_damage_model->parts[chassis + 5].variations.push_back(Component::Model::Variation{
          40, getEngine()->createComponent<PaintedMesh>(car_liscense, liscense_material)});


      // when a region is destroyed we want to detach the shape from the vehicle
      // and spawn a new physics actors object into the game at its location.

      player_vehicle->model = player_damage_model;

      player_vehicle->local_rotation = glm::rotate(3.14159f, glm::vec3(0, 1, 0));
      player_vehicle->local_position = glm::vec3(0.0f, -2.0f, 0.0f);
      player_vehicle->position = glm::vec3(0.0f, 3.0f, -40.0f);

    }
  };
}// namespace Component

#endif// ENGINE_PLAYERVEHICLE_HPP
