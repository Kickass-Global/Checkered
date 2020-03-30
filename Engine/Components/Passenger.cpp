

#include "Passenger.h"
#include <Pipeline/Library.h>
#include <random>

using namespace physx;

void Component::Passenger::setPickupTransform(PxTransform trans) {
  pickupTransform = trans;
}

void Component::Passenger::setDropoffTransform(PxTransform trans) {
  dropOffTransform = trans;
}
void PassengerSystem::update(Engine::deltaTime elapsed) {
  SystemInterface::update(elapsed);

  if (!current_passenger) {

    auto random_unique_pair = [this]() {
      static std::default_random_engine generator(
          std::chrono::steady_clock::now().time_since_epoch().count());
      static std::uniform_int_distribution<int> distribution(
          0, locations.size() - 1);
      int a = distribution(generator);
      int b = distribution(generator);
      while (a == b) {
        b = distribution(generator);
      }
      return std::make_tuple(locations[a], locations[b]);
    };

    auto basic_shader_program =
        getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Program>(
            "Assets/Programs/basic.json");

    auto tree_material =
        getEngine()->createComponent<Component::Material>(basic_shader_program);
    tree_material->textures.push_back(
        getEngine()->createComponent<Component::Texture>(
            "Assets/Textures/Nature_Trees.png"));

    auto tree_mesh =
        getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>(
            "Assets/Meshes/human.obj");

    // make a new passenger...

    auto [pick_up_location, drop_off_location] = random_unique_pair();

    current_passenger = getEngine()->createComponent<Component::Passenger>(
        pick_up_location, drop_off_location, tree_mesh, tree_material);

    current_passenger->onPassengerDroppedOffDelegate += [this](int id) {
      // spawn the next passenger...
      current_passenger.reset();
    };
  }
}
void PassengerSystem::initialize() { SystemInterface::initialize(); }
