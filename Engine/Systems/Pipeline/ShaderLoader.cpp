//
// Created by root on 10/1/20.
//

#include "ShaderLoader.h"
#include <Engine.h>

std::shared_ptr<Rendering::Program> Pipeline::ProgramLoader::load(const std::string &filename) {

    std::ifstream ifs(filename);
    Engine::assertLog<module>(ifs.is_open(), "load program description file " + filename);

    nlohmann::json json;
    ifs >> json;

    Pipeline::ShaderLoader loader;
    std::vector<std::shared_ptr<Rendering::Shader>> shaders;

    for (auto shader : json["entity"]["shaders"]) {
        auto type_id = shader["shader"][0].get<int>();
        auto shader_filename = shader["shader"][1].get<std::string>();

        Engine::log<module>("loading shader: ", shader_filename);

        shaders.push_back(
            std::move(
                type_id == 0 ? loader.load<GL_VERTEX_SHADER>(
                    shader_filename
                ) : loader.load<GL_FRAGMENT_SHADER>(
                    shader_filename
                )));
    }

    return getEngine()->createComponent<Rendering::Program>(shaders);
}
