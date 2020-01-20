//
// Created by root on 9/1/20.
//

#include "RenderingSystem.h"
#include "../../Events.h"

Component::ComponentEvent<int, int> Rendering::RenderingSystem::onWindowSizeChanged;

void Rendering::RenderingSystem::update(Engine::frametime) {

    glClearColor(0, 0, 0.5f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto &&batch : batches) {

        // if batch should be drawn
        {
            // todo: figure out why this is out of sync...
            for(auto&& camera : Component::Index::entitiesOf(Component::ClassId::Camera)) {
                auto camera_is_dirty = Component::Index::hasComponent(camera, Component::Dirty::id());

                if (camera_is_dirty) {
                    auto data = Component::Index::entityData<Component::Camera>(camera);
                    auto view_matrix = glm::toMat4(data->rotation);
                    auto world_matrix = glm::translate(data->position);

                    auto perspective_matrix = glm::perspective(
                            45.0f,
                            static_cast<float>(data->viewport.width) / data->viewport.height,
                            0.1f,
                            100.0f
                    );

                    view_matrix = world_matrix * view_matrix;

                    glViewport(0, 0, data->viewport.width, data->viewport.height);

                    batch->shader.data<Program>()->bind();

                    glUniformMatrix4fv(glGetUniformLocation(batch->shader.data<Program>()->programId(),
                                                            "M_View"),
                                       1, false, glm::value_ptr(view_matrix));

                    glUniformMatrix4fv(glGetUniformLocation(batch->shader.data<Program>()->programId(),
                                                            "M_Perspective"),
                                       1, false, glm::value_ptr(perspective_matrix));

                    Component::Index::removeComponent(camera, Component::Dirty::id());
                }
            }

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

std::shared_ptr<Rendering::RenderBatch> Rendering::RenderingSystem::findSuitableBufferFor(std::shared_ptr<Component::Mesh> data) {

    auto arrayBuffer = std::make_shared<Rendering::BatchBuffer>(
            100000,
            sizeof(data->vertices[0]),
            GL_ARRAY_BUFFER
    );

    auto elementBuffer = std::make_shared<Rendering::BatchBuffer>(
            100000,
            sizeof(data->indices[0]),
            GL_ELEMENT_ARRAY_BUFFER
    );

    auto instanceBuffer = std::make_shared<Rendering::BatchBuffer>(
            100000,
            sizeof(glm::mat4),
            GL_ARRAY_BUFFER
    );

    auto batch = std::make_shared<RenderBatch>(arrayBuffer, elementBuffer, instanceBuffer);
    batch->shader = *Component::Index::entitiesOf(Component::ClassId::Program).begin();

    return push_back(batch);
}

std::shared_ptr<Rendering::RenderBatch> Rendering::RenderingSystem::push_back(
    std::shared_ptr<Rendering::RenderBatch> batch) {
    batches.push_back(batch);
    return batches.back();
}

void Rendering::RenderingSystem::buffer(const Component::Mesh &data) {

    // if the data is already buffered we want to update the existing buffer data
    auto id = data.id();
    auto match = std::find_if(batches.begin(), batches.end(), [id](const auto &batch) {
        return batch->contains(id);
    });

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

    onBillboardModifiedHandler = Engine::EventSystem::createHandler(
            this, &RenderingSystem::onBillboardModified);
}

Rendering::Program::~Program() {
    Engine::log<module>("Deleting program ", m_id);
    glDeleteProgram(m_id);
}

void Rendering::Program::bind() {
    Engine::log<module, Engine::Importance::low>("Binding program ", m_id);
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

    if(!success) {
        GLint maxLength = 0;
        glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(m_id, maxLength, &maxLength, &infoLog[0]);

        Engine::log<module>(std::string(infoLog.begin(), infoLog.end()));
    }
	Engine::assertLog<module>(success != GL_FALSE, "shader creation");
}

GLuint Rendering::Shader::id() const { return m_id; }

Rendering::Shader::Shader() : m_id(static_cast<GLuint>(-1)) {}
