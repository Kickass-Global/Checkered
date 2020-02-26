//
// Created by Jackson Cougar Wiebe on 1/26/2020.
//

#include <map>
#include "pipeline.hpp"

namespace Pipeline {
    namespace {
        std::map<std::string, Component::ClassId> literals = {
                {"program", Component::ClassId::Program},
                {"mesh", Component::ClassId::Mesh},
                {"gameobject", Component::ClassId::GameObject}
        };
    }
}
Component::ClassId Pipeline::parse(const std::string &value) {
    return Pipeline::literals[value];
}
