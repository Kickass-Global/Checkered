//
// Created by root on 22/1/20.
//

#include "Library.h"
#include <Engine.h>

std::map<std::string, std::shared_ptr<ComponentInterface>> Pipeline::Library::assets;

bool Pipeline::Library::contains(const std::string &asset_name) {
    return assets.count(asset_name) > 0;
}

std::pair<std::map<std::string, std::shared_ptr<ComponentInterface>>::iterator, bool>
        Pipeline::Library::emplace(const std::string &asset_name, std::shared_ptr<ComponentInterface> component) {
	Engine::nameComponent(component->getId(), asset_name);
    return assets.emplace(asset_name, component);
}
