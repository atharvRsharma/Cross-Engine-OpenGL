#pragma once

#include "Components.h"
#include "InputManager.h" 



class Physics
{
    

    bool m_gravityEnabled = false;


   
    void solveSpherePlaneCollision(GlowingOrb& orb, Plane& plane);
    void solveCubePlaneCollision(Cube& cube, Plane& plane);


public:
    Physics();

    void update(GlowingOrb& orb, Plane& plane, Cube* cube, const InputState& input, float deltaTime);
    
    void setGravity(bool isEnabled) { m_gravityEnabled = isEnabled; }
    bool getGravityState() const { return m_gravityEnabled; }
};