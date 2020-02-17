//
// Created by Jackson Cougar Wiebe on 2/16/2020.
//
#pragma once
#ifndef ENGINE_COMPONENTID_TPP
#define ENGINE_COMPONENTID_TPP

#include "Index.h"

template<typename T>
T *Component::ComponentId::data() const {
    return Index::entityData<T>(*this);
}

template<typename T>
bool Component::ComponentId::hasTag() const {
    return Index::hasTag<T>(*this, true);
}

template<typename T>
void Component::ComponentId::addTag() const {
    Index::addTag<T>(*this);
}

#endif //ENGINE_COMPONENTID_TPP
