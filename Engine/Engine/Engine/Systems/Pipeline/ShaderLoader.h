//
// Created by root on 10/1/20.
//

#ifndef ENGINE_SHADERLOADER_H
#define ENGINE_SHADERLOADER_H


#include <string>
#include <fstream>
#include <vector>
#include "../../SystemCalls.h"
#include "../Rendering/RenderingSystem.h"

namespace Pipeline {

    constexpr char module[] = "Pipeline";
    constexpr auto assert = system_calls::assert<module>;

    class ShaderLoader {
    public:

        template<int type>
        Rendering::Shader load(std::string filename) {

            std::ifstream ifs(filename);
            assert(ifs.is_open(), "load shader file " + filename);

            std::vector<std::string> lines;
            std::string line;
            while (std::getline(ifs, line)) {
                lines.push_back(line);
            }

            return Rendering::Shader(type, lines);
        }

    };
}

#endif //ENGINE_SHADERLOADER_H
