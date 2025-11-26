#include "Renderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp> 

Renderer::Renderer() {}
Renderer::~Renderer() {}

void Renderer::beginFrame(const Camera& camera, int screenWidth, int screenHeight)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspectRatio = (screenHeight == 0) ? 1.0f : (float)screenWidth / (float)screenHeight;
    m_projection = glm::perspective(glm::radians(camera.Zoom), aspectRatio, 0.1f, 100.0f);
    m_view = camera.GetViewMatrix();
    m_viewPos = camera.Position;
}

void Renderer::endFrame()
{
    
    glBindVertexArray(0);
}

void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}