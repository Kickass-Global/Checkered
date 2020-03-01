//
// Created by Jackson Cougar Wiebe on 1/26/2020.
//

#pragma once

#ifndef ENGINE_PIPELINE_HPP
#define ENGINE_PIPELINE_HPP

#include "../../Components/ComponentId.h"

namespace Pipeline {

    Component::ClassId parse(const std::string &value);

}


#endif //ENGINE_PIPELINE_HPP
