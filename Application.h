#pragma once

#include "Window.h"
#include "Camera.h"
#include "InputManager.h"
#include "Physics.h"
#include "Renderer.h"
#include "Serializer.h"
#include "Components.h"
#include <memory> // For std::unique_ptr

class Application
{
public:
    Application();
    void run();

private:
    // Constants
    const int SCR_WIDTH = 800;
    const int SCR_HEIGHT = 800;
    const char* JSON_PATH = "entity_state.json";
    const char* MODEL_PATH = "sphere.txt";
    const char* VERTEX_SHADER_PATH = "orb.vert";
    const char* FRAGMENT_SHADER_PATH = "orb.frag";

    // Owned objects
    std::unique_ptr<Window> m_window;
    Camera m_camera;
    std::unique_ptr<InputManager> m_input;
    Renderer m_renderer;
    Physics m_physics;
    Serializer m_serializer;

    // The "Entity"
    GlowingOrb m_orb;

    
    
};