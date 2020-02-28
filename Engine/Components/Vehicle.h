//
// Created by root on 17/1/20.
//

#ifndef ENGINE_VEHICLE_H
#define ENGINE_VEHICLE_H

#include <PxPhysicsAPI.h>
#include <vehicle/PxVehicleDrive4W.h>
#include <Events/Events.h>
#include <EventHandler.h>
#include <EventDelegate.h>

#include "Model.h"
#include "ComponentId.h"
#include "tags.h"

#include <glm/glm.hpp>

namespace Component {

class Vehicle : public Component::ComponentBase<Component::ClassId::Vehicle> {
public:
    ComponentId model{};
    ComponentId input{};
    ComponentId onTickHandler;
    EventDelegate<ComponentId> tickHandler = EventDelegate<ComponentId>("handler");

    glm::vec3 scale;
    glm::quat rotation;
    glm::quat local_rotation = glm::rotate(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 position;

    glm::mat4 world_transform() {
        return glm::translate(position) * glm::mat4_cast(rotation) * glm::mat4_cast(local_rotation) * glm::scale(scale);
    }

    bool pxIsVehicleInAir;
    physx::PxVehicleDrive4WRawInputData pxVehicleInputData;
    physx::PxVehicleDrive4W *pxVehicle = nullptr;
    physx::PxVehicleDrivableSurfaceToTireFrictionPairs *pxFrictionPairs = nullptr;
    physx::PxReal pxSteerVsForwardSpeedData[16];
    physx::PxFixedSizeLookupTable<8> pxSteerVsForwardSpeedTable;
    physx::PxVehicleKeySmoothingData pxKeySmoothingData;
    physx::PxVehiclePadSmoothingData pxPadSmoothingData;

    void onTick(const Component::EventArgs<Engine::deltaTime> &args) {
        Engine::log<module>("onTick");
        tickHandler(id());
    }

    Vehicle() :
            pxSteerVsForwardSpeedData{
                    0.0f, 0.75f,
                    5.0f, 0.75f,
                    30.0f, 0.125f,
                    120.0f, 0.1f,
                    PX_MAX_F32, PX_MAX_F32,
                    PX_MAX_F32, PX_MAX_F32,
                    PX_MAX_F32, PX_MAX_F32,
                    PX_MAX_F32, PX_MAX_F32
            },
            pxKeySmoothingData{
                    {
                            6.0f,    //rise rate eANALOG_INPUT_ACCEL
                            6.0f,    //rise rate eANALOG_INPUT_BRAKE
                            6.0f,    //rise rate eANALOG_INPUT_HANDBRAKE
                            2.5f,    //rise rate eANALOG_INPUT_STEER_LEFT
                            2.5f,    //rise rate eANALOG_INPUT_STEER_RIGHT
                    },
                    {
                            10.0f,    //fall rate eANALOG_INPUT_ACCEL
                            10.0f,    //fall rate eANALOG_INPUT_BRAKE
                            10.0f,    //fall rate eANALOG_INPUT_HANDBRAKE
                            5.0f,    //fall rate eANALOG_INPUT_STEER_LEFT
                            5.0f    //fall rate eANALOG_INPUT_STEER_RIGHT
                    }
            },
            pxPadSmoothingData{
                    {
                            6.0f,    //rise rate eANALOG_INPUT_ACCEL
                            6.0f,    //rise rate eANALOG_INPUT_BRAKE
                            6.0f,    //rise rate eANALOG_INPUT_HANDBRAKE
                            2.5f,    //rise rate eANALOG_INPUT_STEER_LEFT
                            2.5f,    //rise rate eANALOG_INPUT_STEER_RIGHT
                    },
                    {
                            10.0f,    //fall rate eANALOG_INPUT_ACCEL
                            10.0f,    //fall rate eANALOG_INPUT_BRAKE
                            10.0f,    //fall rate eANALOG_INPUT_HANDBRAKE
                            5.0f,    //fall rate eANALOG_INPUT_STEER_LEFT
                            5.0f    //fall rate eANALOG_INPUT_STEER_RIGHT
                    }
            } {
        pxSteerVsForwardSpeedTable = physx::PxFixedSizeLookupTable<8>(pxSteerVsForwardSpeedData, 4);
        onTickHandler = Engine::EventSystem::createTickHandler(this, &Vehicle::onTick);
    }

};

}


#endif //ENGINE_VEHICLE_H
