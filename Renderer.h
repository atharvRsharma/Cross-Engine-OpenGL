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

    // Call this once at the start of the render loop
    void beginFrame(const Camera& camera, int screenWidth, int screenHeight);

    // THIS IS THE NEW GENERIC DRAW FUNCTION
    // We pass it the assets, the transform, and a C++ lambda
    // that will set the unique uniforms.
    template<typename Func>
    void draw(Mesh* mesh, Shader* shader, const glm::mat4& model, Func setUniforms)
    {
        if (!mesh || !shader) return;

        shader->use();

        // 1. Set global uniforms (same for everything)
        shader->setMat4("projection", m_projection);
        shader->setMat4("view", m_view);
        shader->setVec3("viewPos", m_viewPos); // Send viewPos, shader can just ignore it

        // 2. Set the model matrix (transform)
        shader->setMat4("model", model);

        // 3. Call the lambda to set specific uniforms
        setUniforms(*shader);

        // 4. Draw the mesh
        mesh->bind();
        glDrawArrays(GL_TRIANGLES, 0, mesh->getVertexCount());
    }

    // Call this at the end of the render loop
    void endFrame();

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

private:
    // Stored matrices for the frame
    glm::mat4 m_projection;
    glm::mat4 m_view;
    glm::vec3 m_viewPos; // Stored camera position
};