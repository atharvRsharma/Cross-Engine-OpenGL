#include "Application.h"
#include <glad/glad.h> 
#include <stdexcept> 


Application::Application()
    : m_camera(glm::vec3(0.0f, 5.0f, 15.0f)) 
{
    m_window = std::make_unique<Window>(SCR_WIDTH, SCR_HEIGHT, "cross engine");
    m_input = std::make_unique<InputManager>(m_window->getNativeWindow(), m_camera);

    glEnable(GL_DEPTH_TEST);
    glfwSetFramebufferSizeCallback(m_window->getNativeWindow(), [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        });

    m_orb = m_serializer.loadState(JSON_PATH);

    m_plane.init();
    
    

    
    m_renderer = std::make_unique<Renderer>();

    m_physics.setGravity(m_orb.isGravityOn);

    m_orbMesh = std::make_unique<Mesh>(ORB_MODEL_PATH);
    m_orbShader = std::make_unique<Shader>(ORB_VERT_PATH, ORB_FRAG_PATH);

    m_planeMesh = std::make_unique<Mesh>(PLANE_MODEL_PATH);
    m_planeShader = std::make_unique<Shader>(BASIC_VERT_PATH, BASIC_FRAG_PATH);


    if (shouldSpawnCube)
    {
        m_cube.init();
        m_cube.scale = glm::vec3(0.2f);
        m_cubeMesh = std::make_unique<Mesh>(CUBE_MODEL_PATH);
        m_cubeShader = std::make_unique<Shader>(BASIC_VERT_PATH, BASIC_FRAG_PATH);
    }

    
}

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

        if (inputState.exitApp) {
            glfwSetWindowShouldClose(m_window->getNativeWindow(), true);
        }
        if (inputState.saveState) {
            m_orb.isGravityOn = m_physics.getGravityState();
            m_serializer.saveState(m_orb, JSON_PATH);
        }
        if (inputState.resetPosition) {
            GlowingOrb defaultOrb;
            m_orb.position = defaultOrb.position;
            m_orb.velocity = defaultOrb.velocity;
        }

        m_physics.update(m_orb, m_plane, (shouldSpawnCube ? &m_cube : nullptr), inputState, deltaTime);

        int width, height;
        m_window->getSize(width, height);

        m_renderer->beginFrame(m_camera, width, height);


        glm::mat4 planeModel = glm::translate(glm::mat4(1.0f), m_plane.position);
        glm::vec3 planeScale(10.0f);
        planeModel = glm::scale(planeModel, planeScale);
        m_renderer->draw(m_planeMesh.get(), m_planeShader.get(), planeModel,
            [&](Shader& shader) {
                shader.setVec3("objectColor", m_plane.color);
            }
        );


        if (shouldSpawnCube) {
            glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), m_cube.position);
            cubeModel = glm::scale(cubeModel, m_cube.scale);

            m_renderer->draw(m_cubeMesh.get(), m_cubeShader.get(), cubeModel,
                [&](Shader& shader) {
                    shader.setVec3("objectColor", m_cube.color);
                }
            );
        }



        glm::mat4 orbModel = glm::translate(glm::mat4(1.0f), m_orb.position);

        m_renderer->draw(m_orbMesh.get(), m_orbShader.get(), orbModel,
            [&](Shader& shader) {
                shader.setFloat("energy", m_orb.energy);
            }
        );




        m_renderer->endFrame();

        m_window->swapBuffers();
        m_window->pollEvents();
    }

    m_orb.isGravityOn = m_physics.getGravityState();


    m_serializer.saveState(m_orb, JSON_PATH); //save struct data to json
}