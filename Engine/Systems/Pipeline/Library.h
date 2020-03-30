//
// Created by root on 22/1/20.
//

#pragma once

#ifndef ENGINE_LIBRARY_H
#define ENGINE_LIBRARY_H

#include <map>
#include <string>
#include <memory>
#include <ComponentBase.h>

#include "MeshLoader.h"
#include "ShaderLoader.h"
#include "ComponentInterface.h"

using Component::ComponentInterface;

namespace Component {
    class Mesh;

    class MeshCollection;
}

namespace Rendering {
    class Program;
}

using Component::Mesh;
using Component::MeshCollection;
using Rendering::Program;

namespace Pipeline {

    class Library : public SystemInterface {
        static std::map<std::string, std::shared_ptr<ComponentInterface>> assets;

    public:
        static bool contains(const std::string &asset_name);

        static std::pair<std::map<std::string, std::shared_ptr<ComponentInterface>>::iterator, bool>
        emplace(const std::string &asset_name, std::shared_ptr<ComponentInterface> component);

        static std::shared_ptr<ComponentInterface> at(const std::string &asset_name) {

            return assets.at(asset_name);
        }

        template <typename T>
        void setAsset(std::string name, T component) {
            emplace(name, component);
        }

        template<typename T>
        std::shared_ptr<T> getAsset(const std::string &name) {

            return std::static_pointer_cast<T>(assets[name]);
        }

        template<>
        std::shared_ptr<Mesh> getAsset<Mesh>(const std::string &name) {

            auto exists = assets.count(name) > 0;
            if (!exists) {
                auto component = getEngine()->getSubSystem<Pipeline::MeshLoader>()->load(name);
                emplace(name, component);
                return component;
            }
            return std::static_pointer_cast<Mesh>(assets[name]);
        }


        template<>
        std::shared_ptr<Program> getAsset<Program>(const std::string &name) {

            auto exists = assets.count(name) > 0;
            if (!exists) {
                auto program = getEngine()->getSubSystem<Pipeline::ProgramLoader>()->load(name);
                emplace(name, program);
                return program;
            }
            return std::static_pointer_cast<Program>(assets[name]);
        }

    };


}

#endif //ENGINE_LIBRARY_H
