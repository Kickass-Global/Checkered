//
// Created by root on 9/1/20.
//

#ifndef ENGINE_RENDERINGSYSTEM_H
#define ENGINE_RENDERINGSYSTEM_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include "../SystemCalls.h"

static char module[] = "RenderingSystem";
const auto assert = system_calls::assert<module>;

class RenderBatch {

    GLuint vao;
    GLuint vertex_data_buffer;
    int size;
    int fill;

public:

    RenderBatch(int bufferMaxSize) : size(bufferMaxSize), fill(0)
    {
        glGenBuffers(1, &vertex_data_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_data_buffer);
        glBufferData(GL_ARRAY_BUFFER, bufferMaxSize, nullptr, GL_STATIC_DRAW);
    }

    void push_back(void* data, int size) {
        glBindBuffer(GL_ARRAY_BUFFER, vertex_data_buffer);
        glBufferSubData(GL_ARRAY_BUFFER, fill, size, data);
        fill += size;
    }

    /**
     * Binds the VAO and sets up all resources needed to draw the geometry in the batch.
     */
    void bind() {
        return;
    }

    /**
     * Calls appropriate glDraw* command to draw the geometry in the batch.
     */
    void draw() {
        return;
    }
};

class RenderingSystem {
    GLFWwindow* window;

    std::vector<RenderBatch> batches;

public:
    void initialize() {

        assert(glfwInit(),"initialize GLFW");

        window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
        glfwMakeContextCurrent(window);

        assert( gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "initialize GLAD");
    }

    void update() {
        glClear(GL_COLOR_BUFFER_BIT);

        for(auto&& batch : batches)
        {
            batch.bind();
            batch.draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ~RenderingSystem() {
        glfwDestroyWindow(window);
    }

private:

};


#endif //ENGINE_RENDERINGSYSTEM_H
