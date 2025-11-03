#pragma once

#include "Window.h"
#include "Camera.h"
#include "InputManager.h"
#include "Physics.h"
#include "Serializer.h"
#include "Components.h"
#include <memory> 


#include "Shader.h"
#include "Mesh.h"
#include "Renderer.h"



class Application
{
public:
    Application();
    void run();

private:

    bool shouldSpawnCube = false;
    
    const char* BASIC_VERT_PATH = "basic.vert";
    const char* BASIC_FRAG_PATH = "basic.frag";

    const char* ORB_VERT_PATH = "orb.vert";
    const char* ORB_FRAG_PATH = "orb.frag";


    const int SCR_WIDTH = 800;
    const int SCR_HEIGHT = 800;
    const char* JSON_PATH = "entity_state.json";

    
    const char* ORB_MODEL_PATH = "sphere.txt";
    const char* PLANE_MODEL_PATH = "plane.txt"; 
    const char* CUBE_MODEL_PATH = "cube.txt"; 
    

    //world systems
    std::unique_ptr<Window> m_window;
    std::unique_ptr<InputManager> m_input;
    std::unique_ptr<Renderer> m_renderer;
    Physics m_physics;
    Serializer m_serializer;
    Camera m_camera;

    //entity
    GlowingOrb m_orb;
    Plane m_plane;
    Cube m_cube;


    //entity descriptors
    std::unique_ptr<Mesh> m_orbMesh;
    std::unique_ptr<Shader> m_orbShader;
    std::unique_ptr<Mesh> m_planeMesh;
    std::unique_ptr<Shader> m_planeShader;
    std::unique_ptr<Mesh> m_cubeMesh;
    std::unique_ptr<Shader> m_cubeShader;
};