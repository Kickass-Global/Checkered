//
// Created by root on 17/1/20.
//

#ifndef ENGINE_ENTITYLOADER_H
#define ENGINE_ENTITYLOADER_H

#include <memory>
#include <string>
#include <type_traits>
#include <cctype>
#include <nlohmann/json.hpp>

#include "../../Components/ComponentId.h"
#include "../../Components/ComponentBase.h"
#include "../../Components/Plane.h"
#include "MeshLoader.h"
#include "pipeline.hpp"
#include "Library.h"
#include "Engine.h"

using json = nlohmann::json;

namespace Pipeline {

    template<typename T = Component::ComponentBase<Component::ClassId::GameObject>>
    std::shared_ptr<T> load(std::string filename) {
        static_assert(std::is_base_of<Component::ComponentInterface, T>::value);

        // walk the description file and load each referenced entity; if
        // the entity has already been loaded we will simply link to the
        // same one (unless its not sharable).

        std::ifstream ifs(filename);
        Engine::assertLog<module>(ifs.is_open(),
                                  "load entity description file " +
                                  filename);

        json config;
        ifs >> config;

        auto name = config["entity"]["name"];
        auto data = config["entity"]["data"];
        auto entity = Engine::createComponent<T>(name);

        // load components ...

        load_meta_data(config, entity);
        load_components(config, entity->id());

        return entity;
    }

    template<class T>
    void load_meta_data(const json &config, T &data);
    void load_components(const json &config, Component::ComponentId entity);
}


#endif //ENGINE_ENTITYLOADER_H
