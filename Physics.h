#pragma once

#include "Components.h"
#include "InputManager.h" // For the InputState

// This class contains all logic for updating the entity's state
class Physics
{
    bool m_gravityEnabled = false;
    const float gravity = -9.8f;
public:
    void update(GlowingOrb& orb, const InputState& input, float deltaTime);
    

};