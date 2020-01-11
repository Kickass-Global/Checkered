// Engine.cpp : Defines the entry point for the application.
//

#include "Systems/Rendering/RenderingSystem.h"
#include "Systems/Pipeline/ShaderLoader.h"
#include "Systems/Rendering/RenderingBatch.h"

int main() {
    auto running = true;

    Rendering::RenderingSystem renderingSystem;
    renderingSystem.initialize();

    // simulate content pipeline loading
    Pipeline::ProgramLoader loader;

    // Component to reference the shader
    Component::Shader shader;

    // load a simple shader program

    renderingSystem.push_back(
            shader.id(),
            loader.load("Assets/Programs/basic.shader")
    );

    Rendering::RenderBatch batch(1000);

    float vertex_data[] = {
            0.9f, -0.9f, 0,
            -0.9f, -0.9f, 0,
            0, 0.9f, 0
    };

    // push batch data and assign shader program to batch
    batch.push_back(vertex_data, sizeof(vertex_data));
    batch.assign_shader(shader);

    renderingSystem.push_back(batch);

    while (running) {
        renderingSystem.update();
    }
}
