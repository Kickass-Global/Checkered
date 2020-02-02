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
#include "Library.h"

using json = nlohmann::json;

namespace Pipeline {

    /**
     * input: entity description file -> output: game-entity
     */

    class EntityLoader {
    public:
        template<typename T = Component::ComponentBase<Component::ClassId::GameObject>>
        static std::shared_ptr<T> load(std::string filename) {
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

            load_components(config, entity->id());

            return entity;
        }

        static void load_components(
                const json &config, const Component::ComponentId &entity) {

            for (auto[key, value] : config["entity"]["components"].items()) {
                Engine::log<module>(key, " ", value);

                auto classId = value["entity"]["class"];
                auto data = value["entity"]["data"];
                auto name = value["entity"]["name"];

                Component::ComponentId componentId;

                if (classId == "mesh") {
                    /* load meshes; */
                    if (Library::contains(data)) {
                        componentId = Library::at(data);
                        Component::Index::addComponent(entity, componentId);
                    } else {
                        MeshLoader loader;
                        auto component = Engine::createComponent(
                                loader.load(data), name);
                        Library::emplace(data, component->id());
                        Component::Index::addComponent(entity, component->id());
                        load_components(value, component->id());
                    }
                } else if (classId == "program") {
                    /*load shaders*/
                    if (Library::contains(data)) {
                        componentId = Library::at(data);
                        Component::Index::addComponent(entity, componentId);
                    } else {
                        ProgramLoader loader;
                        auto component = Engine::createComponent(
                                loader.load(data), name);
                        Library::emplace(data, component->id());
                        Component::Index::addComponent(entity, component->id());
                        load_components(value, component->id());
                    }
                } else if (classId == "quad") {
                    auto quad = Engine::createComponent<Component::Plane>(data);
                }

            }
        }
    };

}


#endif //ENGINE_ENTITYLOADER_H
