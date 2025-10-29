#include "Renderer.h"
#include "FileParser.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cstddef>


Renderer::Renderer() {}

Renderer::~Renderer() {
    // cleanup() is still called manually from Application
}

// MODIFIED: Signature updated
void Renderer::init(const std::string& modelPath, const std::string& vertexPath, const std::string& fragmentPath)
{
    // 1. Compile shaders from files
    m_shader = std::make_unique<Shader>(vertexPath.c_str(), fragmentPath.c_str());

    // 2. Load model
    std::vector<Primitives::Vertex> orbVertices;
    parseObj(modelPath, orbVertices);

    if (orbVertices.empty()) {
        std::cerr << "Failed to load model: " << modelPath << std::endl;
        return;
    }
    m_vertexCount = static_cast<unsigned int>(orbVertices.size());

    // 3. Create VAO/VBO (This logic is unchanged)
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    // ... (rest of VAO/VBO setup is identical)
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
        orbVertices.size() * sizeof(Primitives::Vertex),
        orbVertices.data(),
        GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Primitives::Vertex), (void*)offsetof(Primitives::Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Primitives::Vertex), (void*)offsetof(Primitives::Vertex, texCoord));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Primitives::Vertex), (void*)offsetof(Primitives::Vertex, normal));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::draw(const GlowingOrb& orb, const Camera& camera, int screenWidth, int screenHeight)
{
    // MODIFIED: Check if shader is valid
    if (!m_shader || m_shader->ID == 0 || m_vao == 0) return; // Not initialized

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // MODIFIED: Use the shader class
    m_shader->use();

    // --- Matrices ---
    float aspectRatio = (screenHeight == 0) ? 1.0f : (float)screenWidth / (float)screenHeight;
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), aspectRatio, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, orb.position);

    // MODIFIED: Use shader class setters
    m_shader->setMat4("projection", projection);
    m_shader->setMat4("view", view);
    m_shader->setMat4("model", model);

    // --- Uniforms ---
    m_shader->setFloat("energy", orb.energy);
    m_shader->setVec3("viewPos", camera.Position);

    // --- Draw (Unchanged) ---
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
}

void Renderer::cleanup()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    // The shader is in a unique_ptr, so its destructor
    // will be called automatically, deleting the program.
    m_shader.reset();
}

// --- Static Callback (Unchanged) ---
void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

