#include "scenery.hpp"
#include "ComponentBase.h"
#include "SceneComponent.h"
#include "Engine.h"
#include <Vehicle.h>

void Waypoint::onActorOverlapBegin(
    const EventArgs<PhysicsActor *, PhysicsActor *> &args) {

    std::cout << "player overlap begin waypoint" << std::endl;

    auto player_physics_actor = args.get<0>();
    auto player_vehicle = player_physics_actor->getEngine()->getSubSystem<EngineStore>()
        ->getRoot().getComponentsOfType<ControlledVehicle>()[0];

    onEntered(args.get<1>());
}
void Waypoint::onActorOverlapEnd(
    const EventArgs<PhysicsActor *, PhysicsActor *> &args) {
  onExited(args.get<1>());
}
Waypoint::Waypoint(glm::vec3 position, std::shared_ptr<Mesh> mesh,
                   std::shared_ptr<Material> material)
    : mesh(getEngine()->createComponent<PaintedMesh>(mesh, material)),
      actor(getEngine()->createComponent<PhysicsActor>(mesh)) {

  actor->type = PhysicsActor::Type::TriggerVolume;
  actor->position = position;
  actor->node->getStore().addComponent(
      getEngine()->createComponent<SceneComponent>(actor->node, this->mesh));

  onActorOverlapBeginHandler =
      getEngine()->getSubSystem<EventSystem>()->createHandler(
          this, &Waypoint::onActorOverlapBegin);
  onActorOverlapEndHandler =
      getEngine()->getSubSystem<EventSystem>()->createHandler(
          this, &Waypoint::onActorOverlapEnd);
  actor->onBeginOverlap += onActorOverlapBeginHandler;
  actor->onEndOverlap += onActorOverlapEndHandler;
}
