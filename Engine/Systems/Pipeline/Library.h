//
// Created by root on 22/1/20.
//

#ifndef ENGINE_LIBRARY_H
#define ENGINE_LIBRARY_H

#include <map>
#include <string>

#include "../../Components/Component.h"

namespace Pipeline {

    class Library {
        static std::map<std::string, Component::ComponentId> assets;

    public:
        static bool contains(const std::string &asset_name);

        static void emplace(const std::string &asset_name, Component::ComponentId id);

        static Component::ComponentId at(const std::string& asset_name){
            return assets.at(asset_name);
        }
    };

}

#endif //ENGINE_LIBRARY_H
