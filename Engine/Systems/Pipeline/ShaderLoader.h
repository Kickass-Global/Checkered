//
// Created by root on 10/1/20.
//

#pragma once

#ifndef ENGINE_SHADERLOADER_H
#define ENGINE_SHADERLOADER_H

#include <string>
#include <fstream>
#include <vector>
#include "nlohmann/json.hpp"
#include <Rendering/RenderingSystem.h>

#include <EngineDebug.hpp>


namespace Pipeline {

    namespace {
        char module[] = "Pipeline";
    }

    class ShaderLoader : public SystemInterface {
    public:

        template<int type>
        std::unique_ptr<Rendering::Shader> load(const std::string &filename) {

            std::ifstream ifs(filename);
            Engine::assertLog<module>(ifs.is_open(), "load shader file " + filename);

            std::vector<std::string> lines;
            std::string line;
            while (std::getline(ifs, line)) {
                lines.push_back(line + "\n");
            }

            return std::make_unique<Rendering::Shader>(type, lines);
        }
    };

    class ProgramLoader : public SystemInterface {
    public:
        std::shared_ptr<Rendering::Program> load(const std::string &filename);
    };
}

#endif //ENGINE_SHADERLOADER_H
