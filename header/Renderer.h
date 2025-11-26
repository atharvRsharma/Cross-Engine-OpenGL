#pragma once

#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "Window.h"
#include <glm/glm.hpp>

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void beginFrame(const Camera& camera, int screenWidth, int screenHeight);

    //generic function to draw whatever is thrown at it
    template<typename Func>
    void draw(Mesh* mesh, Shader* shader, const glm::mat4& model, Func setUniforms)
    {
        if (!mesh || !shader) return;

        shader->use();

        shader->setMat4("projection", m_projection);
        shader->setMat4("view", m_view);
        shader->setVec3("viewPos", m_viewPos); 

        shader->setMat4("model", model);

        setUniforms(*shader);

        mesh->bind();
        glDrawArrays(GL_TRIANGLES, 0, mesh->getVertexCount());
    }

    void endFrame();

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

private:
    glm::mat4 m_projection;
    glm::mat4 m_view;
    glm::vec3 m_viewPos; 
};