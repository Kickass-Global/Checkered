//
// Created by root on 22/1/20.
//

#pragma once

#ifndef ENGINE_LIBRARY_H
#define ENGINE_LIBRARY_H

#include <map>
#include <string>

#include "../../Components/Component.h"
#include "../Rendering/RenderingSystem.h"
#include "ShaderLoader.h"

namespace Pipeline {

    class Library {
        static std::map<std::string, Component::ComponentId> assets;

    public:
        static bool contains(const std::string &asset_name);

        static std::pair<std::map<std::string, Component::ComponentId>::iterator, bool>
                emplace(const std::string &asset_name, const Component::ComponentId& id);

        static Component::ComponentId at(const std::string& asset_name){
            return assets.at(asset_name);
        }

        static Component::ComponentId getAsset(const std::string& name, Component::ClassId classId);
    };

}

#endif //ENGINE_LIBRARY_H
