#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "FileParser.h"
#include <string>
#include "Components.h"
#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cstddef>
#include "Shader.h" 

class Renderer
{
public:
    Renderer();
    ~Renderer();

    // MODIFIED: Now takes shader paths
    void init(const std::string& modelPath, const std::string& vertexPath, const std::string& fragmentPath);

    // Draw call (signature stays the same)
    void draw(const GlowingOrb& orb, const Camera& camera, int screenWidth, int screenHeight);

    // Frees OpenGL resources
    void cleanup();

    // Callback for window resizing
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

private:
    // REMOVED: Shader helper functions (they are in Shader.cpp)
    // unsigned int compileShader(...);
    // unsigned int createShaderProgram(...);

    // OpenGL handles
    // MODIFIED: We now own a Shader object
    std::unique_ptr<Shader> m_shader;
    // unsigned int m_shaderProgram = 0; <-- REMOVED

    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    unsigned int m_vertexCount = 0;
};