#include "InputManager.h"

InputManager::InputManager(GLFWwindow* window, Camera& camera)
    : m_window(window), m_camera(camera)
{
    // Store 'this' pointer to be retrieved in static callbacks
    glfwSetWindowUserPointer(m_window, this);

    // Set the callbacks
    glfwSetCursorPosCallback(m_window, mouse_callback);
    glfwSetScrollCallback(m_window, scroll_callback);

    // Capture the mouse
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void InputManager::process(float deltaTime)
{

    m_state.shouldInteract = (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS);


    int escKeyState = glfwGetKey(m_window, GLFW_KEY_ESCAPE);
    m_state.exitApp = (escKeyState == GLFW_PRESS && m_lastEscKeyState == GLFW_RELEASE);
    m_lastEscKeyState = escKeyState;

    // Check for Save ('S' key)
    int sKeyState = glfwGetKey(m_window, GLFW_KEY_X);
    m_state.saveState = (sKeyState == GLFW_PRESS && m_lastSKeyState == GLFW_RELEASE);
    m_lastSKeyState = sKeyState;

    // Check for Gravity Toggle ('G' key)
    int gKeyState = glfwGetKey(m_window, GLFW_KEY_G);
    m_state.toggleGravity = (gKeyState == GLFW_PRESS && m_lastGKeyState == GLFW_RELEASE);
    m_lastGKeyState = gKeyState;

    // Check for Position Reset ('R' key)
    int rKeyState = glfwGetKey(m_window, GLFW_KEY_R);
    m_state.resetPosition = (rKeyState == GLFW_PRESS && m_lastRKeyState == GLFW_RELEASE);
    m_lastRKeyState = rKeyState;




    // --- Poll for camera movement ---
    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
        m_camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        m_camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        m_camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        m_camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
        m_camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        m_camera.ProcessKeyboard(DOWN, deltaTime);

}

// Private mouse callback logic
void InputManager::_mouseCallback(double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (m_firstMouse)
    {
        m_lastX = xpos;
        m_lastY = ypos;
        m_firstMouse = false;
    }

    float xoffset = xpos - m_lastX;
    float yoffset = m_lastY - ypos; // reversed

    m_lastX = xpos;
    m_lastY = ypos;

    m_camera.ProcessMouseMovement(xoffset, yoffset);
}

// Private scroll callback logic
void InputManager::_scrollCallback(double xoffset, double yoffset)
{
    m_camera.ProcessMouseScroll(static_cast<float>(yoffset));
}