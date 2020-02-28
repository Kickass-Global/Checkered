//
// Created by root on 9/1/20.
//

#include <sstream>
#include <Billboard.h>
#include "RenderingSystem.h"

#include "tags.h"
#include "Camera.h"
#include "SceneComponent.h"
#include "Engine.h"
#include "WorldTransform.h"

Component::EventDelegate<int, int> Rendering::RenderingSystem::onWindowSizeChanged("onWindowSizeChanged");

namespace {
const char module[] = "RenderingSystem";
}

void Rendering::RenderingSystem::update(Engine::deltaTime time) {

    std::stringstream ss;
    ss << "frametime: " << time << "ms" << std::endl;
    auto title = ss.str();
    glfwSetWindowTitle(window, title.c_str());



    // Find and update any GameObjects with meshes that should be drawn...
    auto meshes = Component::Index::entitiesOf<Component::Mesh>();
    for (const Component::ComponentId &mesh : meshes) {

        auto is_dirty = mesh.hasTag<Component::Dirty>(true);
        auto is_visible = mesh.hasTag<Component::Visible>(true);

        //if (!is_visible) {
        //    auto match = std::find_if(
        //            batches.begin(), batches.end(), [id = mesh](const auto &batch) {
        //                return batch->contains(id);
        //            }
        //    );

        //    if (match != batches.end()) {
        //        // mark/remove buffered data so its not used
        //        (*match)->remove(mesh);
        //    }
        //}

        auto transforms = mesh.childComponentsOfClass(Component::ClassId::Transform);
        auto is_instanced = !transforms.empty();

        if (is_visible && is_dirty) {

            // buffer the objects meshes (assuming that all meshes should be buffered and drawn).
            Engine::log("Streaming in component#", mesh);

            auto data = mesh.data<Component::Mesh>();

            Engine::log<module, Engine::high>("Updating batch data of#", mesh);
            buffer(*data);

        }
        if (is_instanced) {
            Engine::log<module>("Updating instances of component#", mesh);
            auto classId = mesh.classId();
            std::vector<glm::mat4> transform_data;

            for (auto &&transform : transforms) {
                if (classId == Component::ClassId::Mesh) {
                    Engine::log("Adding instance transform#", transform);
                    transform_data.push_back(transform.data<Component::WorldTransform>()->world_matrix);
                }
            }

            updateInstanceData(
                    mesh,
                    static_cast<int>(sizeof(glm::mat4) * transform_data.size()),
                    (float *) transform_data.data(),
                    sizeof(glm::mat4)
            );
        }

        for (auto &transform : transforms) {
            mesh.destroyComponent(transform);
        }
    }

    glClearColor(0, 0, 0.5f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto &&batch : batches) {
        for (auto &&camera : Component::Index::entitiesOf<Component::Camera>()) {
            auto camera_is_dirty = camera.hasTag<Component::Dirty>(false);

            if (camera_is_dirty) {
                camera.addTag<Component::Dirty>(); // forward this state to the next batch...

                auto data = camera.data<Component::Camera>();
                auto view_matrix = data->view;
                auto world_matrix = glm::translate(data->position);

                auto perspective_matrix = glm::perspective(
                        45.0f,
                        static_cast<float>(data->viewport.width) /
                        data->viewport.height,
                        0.1f,
                        1000.0f
                );

                glViewport(0, 0, data->viewport.width, data->viewport.height);

                batch->shader.data<Program>()->bind();

                glUniformMatrix4fv(
                        glGetUniformLocation(
                                batch->shader.data<Program>()->programId(),
                                "M_View"
                        ),
                        1, false, glm::value_ptr(view_matrix));

                glUniformMatrix4fv(
                        glGetUniformLocation(
                                batch->shader.data<Program>()->programId(),
                                "M_Perspective"
                        ),
                        1, false, glm::value_ptr(perspective_matrix));


            }
        }
    }

    for (auto &&camera : Component::Index::entitiesOf<Component::Camera>()) {
        auto sprites = Component::Index::entitiesOf<Billboard>();
        for (const auto &sprite : sprites) {

            // if this is a new sprite add it to a batch...
            // by attaching a transform that places it at the proper plot location.
            auto camera_view_matrix = camera.data<Camera>()->view;
            auto viewport = camera.data<Camera>()->viewport;
            auto camera_up = camera_view_matrix[1];
            auto camera_right = camera_view_matrix[0];
            auto meta = sprite.data<Billboard>();

            // todo, create a perspective matrix to transform plot to NDC....

            auto offset = glm::translate(
                    glm::vec3(
                            meta->plot.x / (float) viewport.width - 1,
                            meta->plot.y / (float) viewport.height - 1,
                            0.0f
                    ));
            auto
                    scale = glm::scale(
                    glm::vec3(
                            (float) meta->plot.width / viewport.width,
                            (float) meta->plot.height / viewport.height,
                            1.0f
                    ));

            meta->mesh.attachTemporaryComponent(
                    Engine::createComponent<WorldTransform>(offset * scale)->id(), 1
            );
        }

        break; // only use the first camera...
    }

    for (auto &&batch : batches) {
        if (!batch->details.empty()) {
            // bind programs, textures, and uniforms needed to render the batch
            batch->bind(*this);
            batch->draw(*this);
        }
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

Rendering::RenderingSystem::~RenderingSystem() {
    glfwDestroyWindow(window);
}

std::shared_ptr<Rendering::RenderBatch>
Rendering::RenderingSystem::findSuitableBufferFor(
        const std::shared_ptr<Component::Mesh> &data
) {

    auto arrayBuffer = std::make_shared<Rendering::BatchBuffer>(
            10000000,
            sizeof(data->vertices[0]),
            GL_ARRAY_BUFFER
    );

    auto elementBuffer = std::make_shared<Rendering::BatchBuffer>(
            10000000,
            sizeof(data->indices[0]),
            GL_ELEMENT_ARRAY_BUFFER
    );

    auto instanceBuffer = std::make_shared<Rendering::BatchBuffer>(
            10000000,
            sizeof(glm::mat4),
            GL_ARRAY_BUFFER
    );

    auto batch = std::make_shared<RenderBatch>(arrayBuffer, elementBuffer, instanceBuffer);


    batch->shader = data->shader;

    return push_back(batch);
}

std::shared_ptr<Rendering::RenderBatch> Rendering::RenderingSystem::push_back(
        std::shared_ptr<Rendering::RenderBatch> batch
) {
    batches.push_back(batch);
    return batches.back();
}

void Rendering::RenderingSystem::buffer(const Component::Mesh &data) {

    // if the data is already buffered we want to update the existing buffer data
    auto id = data.id();
    auto match = std::find_if(
            batches.begin(), batches.end(), [id](const auto &batch) {
                return batch->contains(id);
            }
    );

    if (match != batches.end()) {
        // mark/remove buffered data so its not used
        (*match)->remove(id);
    }

    // find a batch that can hold the data, or make one
    auto batch = findSuitableBufferFor(std::make_shared<Component::Mesh>(data));

    // todo: if the buffered data size matches that in the buffer we should actually replace it here...
    batch->push_back(data);
}

GLFWwindow *Rendering::RenderingSystem::getWindow() {
    return window;
}

void Rendering::RenderingSystem::windowSizeHandler(GLFWwindow *, int width, int height) {
    onWindowSizeChanged(width, height);
}

void Rendering::RenderingSystem::initialize() {

    Engine::assertLog<module>(glfwInit(), "initialize GLFW");

    window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);

    glfwSetWindowSizeCallback(window, windowSizeHandler);

    glfwMakeContextCurrent(window);

    Engine::assertLog<module>(gladLoadGLLoader((GLADloadproc) glfwGetProcAddress), "initialize GLAD");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);

}

void Rendering::RenderingSystem::updateInstanceData(Component::ComponentId id, int size, float *data, int stride) {

    Engine::log<module>("Updating instance data of component#", id);

    auto it = std::find_if(
            batches.begin(), batches.end(),
            [id](auto batch) { return batch->contains(id); }
    );

    Engine::assertLog<module>(it != batches.end(), "check for valid batch");

    it->get()->update(id, 2, size, data, stride);
}

Rendering::Program::~Program() {
    Engine::log<module>("Deleting program ", m_id);
    glDeleteProgram(m_id);
}

void Rendering::Program::bind() {
    Engine::log<module, Engine::low>("Binding program ", m_id);
    glUseProgram(m_id);
}

GLuint Rendering::Program::programId() {
    return m_id;
}

Rendering::Shader::~Shader() {
    Engine::log<module>("Deleting shader ", m_id);
    glDeleteShader(m_id);
}

Rendering::Shader::Shader(GLenum shader_type, std::vector<std::string> &lines) {

    m_id = glCreateShader(shader_type);
    Engine::log<module>("Creating shader ", m_id);

    std::vector<const GLchar *> cstrings;
    std::vector<int> lengths;

    for (auto &&line : lines) {
        cstrings.push_back(line.c_str());
        lengths.push_back(line.size());
    }

    glShaderSource(m_id, lines.size(), cstrings.data(), lengths.data());
    glCompileShader(m_id);

    GLint success;
    glGetShaderiv(m_id, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLint maxLength = 0;
        glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(m_id, maxLength, &maxLength, &infoLog[0]);

        Engine::log<module, Engine::high>(std::string(infoLog.begin(), infoLog.end()));
    }
    Engine::assertLog<module>(success != GL_FALSE, "shader creation");
}

GLuint Rendering::Shader::glid() const { return m_id; }

Rendering::Shader::Shader() : m_id(static_cast<GLuint>(-1)) {}
