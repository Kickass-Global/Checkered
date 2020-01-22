//
// Created by root on 17/1/20.
//

#ifndef ENGINE_ENTITYLOADER_H
#define ENGINE_ENTITYLOADER_H

#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include "../../Components/ComponentBase.h"

using json = nlohmann::json;

namespace Pipeline {

    /**
     * input: entity description file -> output: game-entity
     */

    class EntityLoader {
    public:
        template<typename T = Component::ComponentBase<Component::ClassId::GameObject>>
        static std::unique_ptr<T> load(std::string filename) {

            // walk the description file and load each referenced entity; if
            // the entity has already been loaded we will simply link to the
            // same one (unless its not sharable).

            std::ifstream ifs(filename);
            Engine::assertLog<module>(ifs.is_open(),
                                      "load entity description file " +
                                      filename);

            json config;
            ifs >> config;

            for (auto component : config["entity"]) {
                Engine::log<module>(component);
            }

            return std::make_unique<T>();
        }
    };

}


#endif //ENGINE_ENTITYLOADER_H
