// Engine.cpp : Defines the entry point for the application.
//

#include "Systems/Rendering/RenderingSystem.h"
#include "Systems/Pipeline/ShaderLoader.h"
#include "Systems/Rendering/RenderingBatch.h"

int main()
{
    auto running = true;

    Rendering::RenderingSystem renderingSystem;
    renderingSystem.initialize();

    // simulate content pipeline loading

    Rendering::RenderBatch batch(1000);

    float vertex_data[] = {
            0.5f,0,0,
            -0.5f,0,0,
            0,1,0
    };

    Pipeline::ShaderLoader loader;

    // Component to reference the shader
    Component::Shader shader;

    // load a simple shader program
    {
        auto vertex_shader = loader.load<GL_VERTEX_SHADER>("Assets/Shaders/basic.vert");
        auto fragment_shader = loader.load<GL_FRAGMENT_SHADER>("Assets/Shaders/basic.frag");

        renderingSystem.push_back(
                shader.id(),
                std::make_unique<Rendering::Program>(vertex_shader, fragment_shader)
        );
    }

    // push batch data and assign shader program to batch
    batch.push_back(vertex_data, sizeof(vertex_data));
    batch.assign_shader(shader);

    renderingSystem.push_back(batch);

    while(running)
    {
        renderingSystem.update();
    }
}
