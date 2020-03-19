//
// Created by Jackson Cougar Wiebe on 3/17/2020.
//

#include "Node.hpp"

Component::NodeEntry::NodeEntry(bool momentary,
                                int ttl,
                                std::shared_ptr<class ComponentBase> component)
    : momentary(momentary), ttl(ttl), component(std::move(component)) {}