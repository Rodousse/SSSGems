#define GLEW_STATIC
#include <array>
#include <chrono>
#include <defines.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "utils/ObjLoader.h"
#include "utils/ShaderUtils.h"


struct UniformData
{
    glm::mat4 proj;
    glm::mat4 view;
};

struct WindowData
{
    GLFWwindow* window;
    int width;
    int height;
    bool resized;
} winData;

void actualizeProjection(glm::mat4& projMat, const WindowData& window);
void actualizeView(glm::mat4& viewMat);
void resizeCallback(GLFWwindow* win, int width, int height);

int main()
{
    if(!glfwInit())
    {
        std::cerr << "ERROR: could not start GLFW" << std::endl;
        return 1;
    }

    winData.window = glfwCreateWindow(640, 480, "Hello Triangle", nullptr, nullptr);
    winData.width = 640;
    winData.height = 480;

    glfwSetWindowSizeCallback(winData.window, resizeCallback);

    if(!winData.window)
    {
        std::cerr << "ERROR: could not open window with GLFW3" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(winData.window);

    if(glewInit() != GLEW_OK)
    {
        std::cerr << "ERROR: could not start GLEW" << std::endl;
        return 1;
    }

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "Renderer: " <<  renderer << std::endl;
    std::cout << "OpenGL version supported " <<  version << std::endl;

    Mesh mesh;

    if(!loader::load(std::string(RESOURCE_PATH) + "/models/dragon.obj", mesh))
    {
        throw std::runtime_error("Can't load file : " + std::string(RESOURCE_PATH) + "/models/dragon.obj");
    }

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(),
                 GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, normal)));

    GLuint ebo = 0;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(),
                 GL_STATIC_DRAW);

    auto vertex_shader = shader::loadShader(std::string(RESOURCE_PATH) + "/shaders/vertex.vert");
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* vertexDataPointer = vertex_shader.data();
    const GLint vertexDataSize = vertex_shader.size();
    glShaderSource(vs, 1, &vertexDataPointer, &vertexDataSize);
    glCompileShader(vs);

    auto fragment_shader = shader::loadShader(std::string(RESOURCE_PATH) + "/shaders/fragment.frag");
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* fragmentDataPointer = fragment_shader.data();
    const GLint fragmentDataSize = fragment_shader.size();
    glShaderSource(fs, 1, &fragmentDataPointer, &fragmentDataSize);
    glCompileShader(fs);

    GLuint shader_programme = glCreateProgram();
    glAttachShader(shader_programme, fs);
    glAttachShader(shader_programme, vs);
    glLinkProgram(shader_programme);

    UniformData ubo;
    actualizeProjection(ubo.proj, winData);
    actualizeView(ubo.view);

    GLuint projID = glGetUniformLocation(shader_programme, "VP.proj");
    GLuint viewID = glGetUniformLocation(shader_programme, "VP.view");

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);


    while(!glfwWindowShouldClose(winData.window))
    {
        // wipe the drawing surface clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_programme);

        if(winData.resized)
        {
            actualizeProjection(ubo.proj, winData);
            winData.resized = false;
        }

        actualizeView(ubo.view);
        glUniformMatrix4fv(projID, 1, false, &ubo.proj[0][0]);
        glUniformMatrix4fv(viewID, 1, false, &ubo.view[0][0]);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, nullptr);
        // update other events like input handling
        glfwPollEvents();
        // put the stuff we've been drawing onto the display
        glfwSwapBuffers(winData.window);
    }

    glfwTerminate();
    return 0;
}

void actualizeProjection(glm::mat4& projMat, const WindowData& data)
{
    projMat = glm::perspective(45.0f, float(data.width) / float(data.height), 0.01f, 100.0f);
}

void actualizeView(glm::mat4& viewMat)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto delay = std::chrono::duration_cast<std::chrono::duration<float>>
                 (std::chrono::high_resolution_clock::now() -
                  startTime);
    float angleRotation = delay.count() * PI / 4.0f;
    viewMat = glm::lookAt(glm::vec3(2.0 * std::cos(angleRotation), 0.0, 2.0 * std::sin(angleRotation)),
                          glm::vec3(0.0),
                          glm::vec3(0.0, 1.0, 0.0));
}

void resizeCallback(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
    winData.resized = true;
    winData.width = width;
    winData.height = height;
}

