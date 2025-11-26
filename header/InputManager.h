#pragma once

#include "Camera.h"
#include <GLFW/glfw3.h>

// A simple struct to hold the results of our input polling
struct InputState {
    bool shouldInteract = false;
    bool toggleGravity = false; 
    bool resetPosition = false; 

   
    bool saveState = false;
    bool exitApp = false;
    
};

// Manages all GLFW input and updates the camera
class InputManager
{
public:
    InputManager(GLFWwindow* window, Camera& camera);

    // Polls key presses
    void process(float deltaTime);

    // Getters
    const InputState& getState() const { return m_state; }

private:
    // Member versions of the callbacks
    void _mouseCallback(double xpos, double ypos);
    void _scrollCallback(double xoffset, double yoffset);

    // Static C-style "trampoline" functions
    // These are called by GLFW and then forward the call to our class instance
    static void mouse_callback(GLFWwindow* w, double x, double y) {
        static_cast<InputManager*>(glfwGetWindowUserPointer(w))->_mouseCallback(x, y);
    }
    static void scroll_callback(GLFWwindow* w, double x, double y) {
        static_cast<InputManager*>(glfwGetWindowUserPointer(w))->_scrollCallback(x, y);
    }

    // Member variables
    GLFWwindow* m_window;
    Camera& m_camera;
    InputState m_state;

    float m_lastX = 400.0f;
    float m_lastY = 300.0f;
    bool m_firstMouse = true;
    int m_lastGKeyState = GLFW_RELEASE;
    int m_lastRKeyState = GLFW_RELEASE;
    int m_lastSKeyState = GLFW_RELEASE;
    int m_lastEscKeyState = GLFW_RELEASE;
};