#define GLEW_STATIC
#include <defines.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader/ShaderUtils.h"
#include <chrono>
#include <array>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
};


constexpr glm::vec3 points[3] =
{
    {0.0f,  0.5f,  0.0f},
    {0.5f, -0.5f,  0.0f},
    {-0.5f, -0.5f,  0.0f}
};

constexpr std::array<Vertex, 3> vertices =
{
    {   {0.0f,  0.5f,  0.0f}, {1.0f, 0.0f, 0.0f}},
    {glm::vec3(0.5f, -0.5f,  0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
    {glm::vec3(-0.5f, -0.5f,  0.0f), glm::vec3(0.0f, 0.0f, 1.0f)}
};

int main()
{
    if(!glfwInit())
    {
        std::cerr << "ERROR: could not start GLFW" << std::endl;
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello Triangle", nullptr, nullptr);

    if(!window)
    {
        std::cerr << "ERROR: could not open window with GLFW3" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

//    glewExperimental = GL_TRUE;

    if(glewInit() != GLEW_OK)
    {
        std::cerr << "ERROR: could not start GLEW" << std::endl;
        return 1;
    }

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "Renderer: " <<  renderer << std::endl;
    std::cout << "OpenGL version supported " <<  version << std::endl;

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

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

    glm::mat4 projection = glm::perspective(45.0, 640.0 / 480.0, 0.01, 100.0);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0, .0, 2.0), glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 model = glm::mat4(1.0);

    glm::mat4 mvp = projection * view * model;

    GLuint mvpID = glGetUniformLocation(shader_programme, "MVP");

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    auto startTime = std::chrono::high_resolution_clock::now();

    while(!glfwWindowShouldClose(window))
    {
        // wipe the drawing surface clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_programme);
        glUniformMatrix4fv(mvpID, 1, false, &mvp[0][0]);

        auto delay = std::chrono::duration_cast<std::chrono::duration<float>>
                     (std::chrono::high_resolution_clock::now() -
                      startTime);
        view = glm::lookAt(glm::vec3(2.0 * std::cos(delay.count() * PI),
                                     0.0,
                                     2.0 * std::sin(delay.count() * PI)),
                           glm::vec3(0.0),
                           glm::vec3(0.0, 1.0, 0.0));

        mvp = projection * view * model;


        glBindVertexArray(vao);
        // draw points 0-3 from the currently bound VAO with current in-use shader
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // update other events like input handling
        glfwPollEvents();
        // put the stuff we've been drawing onto the display
        glfwSwapBuffers(window);
    }


    glfwTerminate();
    return 0;
}
