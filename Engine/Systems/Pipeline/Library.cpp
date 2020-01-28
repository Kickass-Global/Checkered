//
// Created by root on 22/1/20.
//

#include "Library.h"
#include "EntityLoader.h"

std::map<std::string, Component::ComponentId> Pipeline::Library::assets;

bool Pipeline::Library::contains(const std::string &asset_name) {
    return assets.count(asset_name) > 0;
}

std::pair<std::map<std::string, Component::ComponentId>::iterator, bool>
        Pipeline::Library::emplace(const std::string &asset_name, const Component::ComponentId& id) {
    return assets.emplace(asset_name, id);
}

Component::ComponentId Pipeline::Library::getAsset(const std::string &name, Component::ClassId classId) {
    auto exists = assets.count(name) > 0;
    if (!exists) {
        switch (classId) {
            case Component::ClassId::Program: {
                auto program = Pipeline::ProgramLoader::load(name);
                Engine::createComponent(program, name);
                return program->id();
            }
            case Component::ClassId::Mesh: {
                auto component = Pipeline::MeshLoader::load(name);
                Engine::createComponent(component, name);
                return component->id();
            }
        }
    }
    return assets[name];
}
