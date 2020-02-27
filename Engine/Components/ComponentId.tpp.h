//
// Created by Jackson Cougar Wiebe on 2/16/2020.
//
#pragma once
#ifndef ENGINE_COMPONENTID_TPP
#define ENGINE_COMPONENTID_TPP

#include "Index.h"

template<typename T>
T *Component::ComponentId::data() const {
    if (!data_ptr) { // cache the pointer
        data_ptr = Index::entityData<T>(*this);
    }
    return reinterpret_cast<T *>(data_ptr);
}

template<typename T>
bool Component::ComponentId::hasTag(bool clear) const {
    return Index::hasTag<T>(*this, clear);
}

template<typename T>
void Component::ComponentId::addTag() const {
    Index::addTag<T>(*this);
}

#endif //ENGINE_COMPONENTID_TPP
