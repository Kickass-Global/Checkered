//
// Created by root on 22/1/20.
//

#include "Library.h"

std::map<std::string, Component::ComponentId> Pipeline::Library::assets;

bool Pipeline::Library::contains(const std::string &asset_name) {
    return assets.count(asset_name) > 0;
}

void Pipeline::Library::emplace(const std::string &asset_name, Component::ComponentId id) {
    assets[asset_name] = id;
}
