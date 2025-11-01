#include "Application.h"
#include <glad/glad.h> // For glEnable, glViewport
#include <stdexcept> 

// --- CONSTRUCTOR ---
Application::Application()
    : m_camera(glm::vec3(0.0f, 5.0f, 15.0f)) // Start camera 5 units up
{
    // --- 1. Initialize Window & Input ---
    m_window = std::make_unique<Window>(SCR_WIDTH, SCR_HEIGHT, "OpenGL Simulation");
    m_input = std::make_unique<InputManager>(m_window->getNativeWindow(), m_camera);

    // --- 2. Initialize OpenGL State ---
    glEnable(GL_DEPTH_TEST);
    // Set the resize callback
    glfwSetFramebufferSizeCallback(m_window->getNativeWindow(), [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        });

    // --- 3. Load Entity Data ---
    m_orb = m_serializer.loadState(JSON_PATH);

    m_orb.init();    
    m_plane.init();
    m_cube.init();

    m_cube.scale = glm::vec3(0.2f);
    // --- 4. Initialize Systems ---
    m_renderer = std::make_unique<Renderer>();
    // m_physics and m_serializer are already created (stack allocated)

    // --- 5. Load Assets ---
    m_orbMesh = std::make_unique<Mesh>(ORB_MODEL_PATH);
    m_orbShader = std::make_unique<Shader>(ORB_VERT_PATH, ORB_FRAG_PATH);

    // Un-commented these lines
    m_planeMesh = std::make_unique<Mesh>(PLANE_MODEL_PATH);
    m_planeShader = std::make_unique<Shader>(BASIC_VERT_PATH, BASIC_FRAG_PATH);

    m_cubeMesh = std::make_unique<Mesh>(CUBE_MODEL_PATH);
    m_cubeShader = std::make_unique<Shader>(BASIC_VERT_PATH, BASIC_FRAG_PATH);
}

// --- RUN LOOP ---
void Application::run()
{
    float lastFrame = 0.0f;

    while (!m_window->shouldClose())
    {
        
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        
        m_input->process(deltaTime);
        InputState inputState = m_input->getState();

        // Handle app-level inputs
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

        // --- 3. Physics System Update ---
        m_physics.update(m_orb, m_plane, &m_cube, inputState, deltaTime);

        // --- 4. Render System Update ---
        int width, height;
        m_window->getSize(width, height);

        // Tell the renderer to clear the screen and set up cameras
        m_renderer->beginFrame(m_camera, width, height);

        // --- RENDER COMMANDS (THE "DRIVER" LOGIC) ---

        // --- 4a. RENDER THE PLANE ---
        // Calculate the plane's transform
        glm::mat4 planeModel = glm::translate(glm::mat4(1.0f), m_plane.position);
        glm::vec3 planeScale(10.0f);
        planeModel = glm::scale(planeModel, planeScale);
        // Call the generic draw function
        m_renderer->draw(m_planeMesh.get(), m_planeShader.get(), planeModel,
            // Pass a C++ lambda (a mini-function) to set plane-specific uniforms
            [&](Shader& shader) {
                shader.setVec3("objectColor", m_plane.color);
            }
        );

        glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), m_cube.position);
        cubeModel = glm::scale(cubeModel, m_cube.scale);

        m_renderer->draw(m_cubeMesh.get(), m_cubeShader.get(), cubeModel,
            // Pass a C++ lambda (a mini-function) to set plane-specific uniforms
            [&](Shader& shader) {
                shader.setVec3("objectColor", m_cube.color);
            }
        );





        // --- 4b. RENDER THE ORB ---
        // Calculate the orb's transform
        glm::mat4 orbModel = glm::translate(glm::mat4(1.0f), m_orb.position);

        // Call the generic draw function
        m_renderer->draw(m_orbMesh.get(), m_orbShader.get(), orbModel,
            // Pass a different lambda to set orb-specific uniforms
            [&](Shader& shader) {
                shader.setFloat("energy", m_orb.energy);
                // Note: The generic draw() already sets "viewPos" for all objects
            }
        );

        // Tell the renderer we're done drawing
        m_renderer->endFrame();

        // --- 5. End of Frame ---
        m_window->swapBuffers();
        m_window->pollEvents();
    }

    // --- Cleanup ---
    m_serializer.saveState(m_orb, JSON_PATH); // Final save
    // All unique_ptrs will automatically delete themselves and free memory
}