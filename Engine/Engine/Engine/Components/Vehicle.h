//
// Created by root on 17/1/20.
//

#ifndef ENGINE_VEHICLE_H
#define ENGINE_VEHICLE_H


#include "Model.h"

namespace Component {

class Vehicle {
    ComponentId model;
    ComponentId input;
    ComponentId physicsModel;
};

}


#endif //ENGINE_VEHICLE_H
