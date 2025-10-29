#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Renderer.h"
#include <string>

class Window
{
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    // No copying or assigning
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() const { return glfwWindowShouldClose(m_window); }
    void swapBuffers() { glfwSwapBuffers(m_window); }
    void pollEvents() { glfwPollEvents(); }

    GLFWwindow* getNativeWindow() const { return m_window; }
    void getSize(int& width, int& height) const { glfwGetFramebufferSize(m_window, &width, &height); }

private:
    GLFWwindow* m_window = nullptr;
};