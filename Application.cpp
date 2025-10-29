#include "Application.h"
#include <glad/glad.h>
#include <stdexcept> // For runtime_error



Application::Application()
    : m_camera(glm::vec3(0.0f, 0.0f, 15.0f))
{
    // Window must be created first to get GL context
    m_window = std::make_unique<Window>(SCR_WIDTH, SCR_HEIGHT, "huhuhhuhuhuhuhuh");

    // Input system needs the native window
    m_input = std::make_unique<InputManager>(m_window->getNativeWindow(), m_camera);

    // Set up OpenGL state
    glEnable(GL_DEPTH_TEST);

    // Load entity state
    m_orb = m_serializer.loadState(JSON_PATH);

    // Initialize renderer (needs GL context)
    m_renderer.init(MODEL_PATH, VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
}




void Application::run()
{
    float lastFrame = 0.0f;

    while (!m_window->shouldClose())
    {
        // Delta time
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // --- Input ---
        m_input->process(deltaTime);
        InputState inputState = m_input->getState(); // Get state

        if (inputState.exitApp) {
            glfwSetWindowShouldClose(m_window->getNativeWindow(), true);
        }

        if (inputState.saveState) {
            m_serializer.saveState(m_orb, JSON_PATH);
        }

        if (inputState.resetPosition) {
            GlowingOrb defaultOrb;
            m_orb.position = defaultOrb.position;
            m_orb.velocity = defaultOrb.velocity;
        }


       

        // --- Update ---
        m_physics.update(m_orb, inputState, deltaTime);

        // --- Render ---
        int width, height;
        m_window->getSize(width, height);
        m_renderer.draw(m_orb, m_camera, width, height);

        

        // --- End of frame ---
        m_window->swapBuffers();
        m_window->pollEvents();
    }

    // --- Cleanup ---
    m_serializer.saveState(m_orb, JSON_PATH); // Final save
    m_renderer.cleanup();
    // m_window's destructor handles glfwTerminate
}