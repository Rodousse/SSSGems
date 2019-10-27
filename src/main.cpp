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

#define SHADOW_RES 2048


struct UniformData
{
    glm::mat4 proj;
    glm::mat4 view;
    glm::mat4 projView;
};

struct WindowData
{
    GLFWwindow* window;
    int width;
    int height;
    bool resized;
} winData;

struct DirectionnalLightInfo
{
    glm::vec3 dir;
    glm::mat4 biasProjView;
    UniformData ubo;
    float size;
};

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

    winData.window = glfwCreateWindow(640, 480, "SSSGems", nullptr, nullptr);
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

    if(!loader::load(std::string(RESOURCE_PATH) + "/models/bunny.obj", mesh))
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


    /*
     * Depth pipeline
     */
    auto vertexDepthShader = shader::loadShader(std::string(RESOURCE_PATH) +
                             "/shaders/vertexDepth.vert");
    GLuint vds = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* vertexDepthDataPointer = vertexDepthShader.data();
    const GLint vertexDepthDataSize = vertexDepthShader.size();
    glShaderSource(vds, 1, &vertexDepthDataPointer, &vertexDepthDataSize);
    glCompileShader(vds);

    auto fragmentDepthShader = shader::loadShader(std::string(RESOURCE_PATH) +
                               "/shaders/fragmentDepth.frag");
    GLuint fds = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* fragmentDepthDataPointer = fragmentDepthShader.data();
    const GLint fragmentDepthDataSize = fragmentDepthShader.size();
    glShaderSource(fds, 1, &fragmentDepthDataPointer, &fragmentDepthDataSize);
    glCompileShader(fds);

    GLuint shaderDepthProgramme = glCreateProgram();
    glAttachShader(shaderDepthProgramme, fds);
    glAttachShader(shaderDepthProgramme, vds);
    glLinkProgram(shaderDepthProgramme);


    DirectionnalLightInfo light;
    light.dir = glm::normalize(glm::vec3(0.0f, -1.0f, -1.0f));
    light.size = 5.0f;
    light.ubo.proj = glm::ortho(-light.size / 2.0f, light.size / 2.0f, -light.size / 2.0f,
                                light.size / 2.0f, -10.0f, 20.0f);
    light.ubo.view = glm::lookAt(-light.dir * 1.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f,
                                 0.0f));
    light.ubo.projView = light.ubo.proj * light.ubo.view;


    GLint projDepthID = glGetUniformLocation(shaderDepthProgramme, "camera.proj");
    GLint viewDepthID = glGetUniformLocation(shaderDepthProgramme, "camera.view");


    glUniformMatrix4fv(projDepthID, 1, false, &light.ubo.proj[0][0]);
    glUniformMatrix4fv(viewDepthID, 1, false, &light.ubo.view[0][0]);

    //We generate a framebuffer, and will render the color rendertarget and depth buffer
    GLuint framebufferDepth;
    glGenFramebuffers(1, &framebufferDepth);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferDepth);

    // Color
    GLuint texDepth;
    glGenTextures(1, &texDepth);
    glBindTexture(GL_TEXTURE_2D, texDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SHADOW_RES, SHADOW_RES, 0, GL_RGBA, GL_FLOAT,
                 nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texDepth, 0);

    // Depth Attachment
    GLuint texShadow;
    glGenTextures(1, &texShadow);
    glBindTexture(GL_TEXTURE_2D, texShadow);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, SHADOW_RES, SHADOW_RES, 0, GL_DEPTH_COMPONENT,
                 GL_FLOAT,
                 nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texShadow, 0);

    std::array<GLenum, 2> drawBuffers = {GL_COLOR_ATTACHMENT0, GL_NONE};

    glDrawBuffers(3, drawBuffers.data());

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Failed to create framebuffer from depth attachment!");


    /*
     *  Main pipeline
     */
    auto vertexShader = shader::loadShader(std::string(RESOURCE_PATH) + "/shaders/vertex.vert");
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* vertexDataPointer = vertexShader.data();
    const GLint vertexDataSize = vertexShader.size();
    glShaderSource(vs, 1, &vertexDataPointer, &vertexDataSize);
    glCompileShader(vs);

    auto fragmentShader = shader::loadShader(std::string(RESOURCE_PATH) + "/shaders/fragment.frag");
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* fragmentDataPointer = fragmentShader.data();
    const GLint fragmentDataSize = fragmentShader.size();
    glShaderSource(fs, 1, &fragmentDataPointer, &fragmentDataSize);
    glCompileShader(fs);

    GLuint shaderProgramme = glCreateProgram();
    glAttachShader(shaderProgramme, fs);
    glAttachShader(shaderProgramme, vs);
    glLinkProgram(shaderProgramme);

    UniformData ubo;
    actualizeProjection(ubo.proj, winData);
    actualizeView(ubo.view);

    GLint projID = glGetUniformLocation(shaderProgramme, "camera.proj");
    GLint viewID = glGetUniformLocation(shaderProgramme, "camera.view");
    GLint lightBiasProjViewID = glGetUniformLocation(shaderProgramme, "light.biasProjView");
    GLint lightProjViewID = glGetUniformLocation(shaderProgramme, "light.projView");
    GLint lightViewID = glGetUniformLocation(shaderProgramme, "light.view");
    GLint lightDirID = glGetUniformLocation(shaderProgramme, "light.dir");
    GLint texDepthID  = glGetUniformLocation(shaderProgramme, "texDepth");
    GLint texShadowID  = glGetUniformLocation(shaderProgramme, "texShadow");

    glm::mat4 biasMatrix(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    );
    light.biasProjView = biasMatrix * light.ubo.projView;


    /**********************************************************************************/

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    while(!glfwWindowShouldClose(winData.window))
    {
        // wipe the drawing surface clear

        // Depth framebuffer computation
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferDepth);
        glViewport(0, 0, SHADOW_RES, SHADOW_RES);
        glUseProgram(shaderDepthProgramme);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glUniformMatrix4fv(projDepthID, 1, false, &light.ubo.proj[0][0]);
        glUniformMatrix4fv(viewDepthID, 1, false, &light.ubo.view[0][0]);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, nullptr);


        // Main framebuffer Computation
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, winData.width, winData.height);
        glUseProgram(shaderProgramme);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        if(winData.resized)
        {
            actualizeProjection(ubo.proj, winData);
            winData.resized = false;
        }

        actualizeView(ubo.view);
        glUniformMatrix4fv(projID, 1, false, &ubo.proj[0][0]);
        glUniformMatrix4fv(viewID, 1, false, &ubo.view[0][0]);
        glUniformMatrix4fv(lightBiasProjViewID, 1, false, &light.biasProjView[0][0]);
        glUniformMatrix4fv(lightProjViewID, 1, false, &light.ubo.projView[0][0]);
        glUniformMatrix4fv(lightViewID, 1, false, &light.ubo.view[0][0]);
        glUniform3fv(lightDirID, 1, &light.dir[0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texDepth);
        glUniform1i(texDepthID, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texShadow);
        glUniform1i(texShadowID, 1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, nullptr);

        // update other events like input handling
        glfwPollEvents();
        // put the stuff we've been drawing onto the display
        glfwSwapBuffers(winData.window);
    }

    glDeleteFramebuffers(1, &framebufferDepth);

    glfwTerminate();
    return 0;
}

// Refresh projection infos
void actualizeProjection(glm::mat4& projMat, const WindowData& data)
{
    projMat = glm::perspective(45.0f, float(data.width) / float(data.height), 0.01f, 100.0f);
}

// New camera position and orientation
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

//Handle the window resizing
void resizeCallback(GLFWwindow*, int width, int height)
{
    winData.resized = true;
    winData.width = width;
    winData.height = height;
}

