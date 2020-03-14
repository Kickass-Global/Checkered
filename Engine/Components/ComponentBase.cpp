//
// Created by root on 15/1/20.
//

#include "ComponentBase.h"
#include "ComponentInterface.h"

#include <utility>

Component::NodeEntry::NodeEntry(int momentary, int ttl, std::shared_ptr<struct ComponentBase> component) : momentary(
    momentary
), ttl(ttl), component(std::move(component)) {}
