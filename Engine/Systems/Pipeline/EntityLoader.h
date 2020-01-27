//
// Created by root on 17/1/20.
//

#ifndef ENGINE_ENTITYLOADER_H
#define ENGINE_ENTITYLOADER_H

#include <memory>
#include <string>
#include <type_traits>
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

            auto entity = Engine::createComponent<T>(filename);

            for (auto[key, value] : config["entity"].items()) {
                Engine::log<module>(key, " ", value);

                if (key == "mesh") {
                    /* load meshes; */
                    if (Library::contains(value)) {
                        auto id = Library::at(value);
                        Component::Index::addComponent(entity->id(), id);
                    } else {
                        MeshLoader loader;
                        auto component = Engine::createComponent(loader.load(value), value);
                        Library::emplace(value, component->id());
                        Component::Index::addComponent(entity->id(), component->id());
                    }
                } else if (key == "shader") {
                    /*load shaders*/
                    if (Library::contains(value)) {
                        auto id = Library::at(value);
                        Component::Index::addComponent(entity->id(), id);
                    } else {
                        ProgramLoader loader;
                        auto component = Engine::createComponent(loader.load(value), value);
                        Library::emplace(value, component->id());
                        Component::Index::addComponent(entity->id(), component->id());
                    }
                }
                else if (key == "quad")
                {
                    auto quad = Engine::createComponent<Component::Plane>(value);
                    entity->id();
                }
            }

            return entity;
        }
    };

}


#endif //ENGINE_ENTITYLOADER_H
