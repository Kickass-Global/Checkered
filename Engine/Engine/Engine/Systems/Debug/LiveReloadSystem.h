//
// Created by root on 15/1/20.
//

#ifndef ENGINE_LIVERELOADSYSTEM_H
#define ENGINE_LIVERELOADSYSTEM_H

#include <string>
#include <filesystem>
#include <map>
#include "../../main.h"
#include "../../SystemCalls.h"
#include "../../Engine.h"
#include "../../Events.h"

namespace Debug {

    inline static char module[] = "Debug";


/**
 * This system will handle watching assets on disk and reloading them while the game is already running
 */
    class LiveReloadSystem {

        struct AssetDetails {
            std::filesystem::file_time_type last_modified_time;
            Component::ClassId classId;
            Component::ComponentId componentId;
        };

        std::map<std::string, AssetDetails> modified;

    public:

        Engine::Event<std::string, Component::ClassId, Component::ComponentId> onAssetModified;

        void watch(const std::string &filename, Component::ClassId classId, Component::ComponentId componentId) {
            modified[filename] = {std::filesystem::last_write_time(filename), classId, componentId};
        }

        void update(Engine::frametime /*elapsed*/) {

            for (const auto&[filename, details] : modified) {
                try {
                    auto last_modified_time = std::filesystem::last_write_time(filename);
                    if (last_modified_time != details.last_modified_time) {

                        modified[filename].last_modified_time = last_modified_time;

                        Engine::log<module>("Asset was modified: ",
                                filename, details.classId, details.componentId);

                        onAssetModified(filename, details.classId, details.componentId);

                    }
                } catch (std::exception &/*ignored*/) {
                    continue;
                }
            }
        }
    };

}

#endif //ENGINE_LIVERELOADSYSTEM_H
