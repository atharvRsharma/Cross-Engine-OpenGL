#include "Physics.h"
#include <glm/glm.hpp>
#include <algorithm> 



void Physics::update(GlowingOrb& orb, const InputState& input, float deltaTime)
{
    if (input.toggleGravity) {
        m_gravityEnabled = !m_gravityEnabled; 
    }

    
    if (input.shouldInteract) {
        orb.energy += 1.0f * deltaTime; 
        orb.energy = glm::min(orb.energy, 1.0f); 
    }
    
    else {
        orb.energy -= 0.1f * deltaTime; 
        orb.energy = glm::max(orb.energy, 0.0f); 
    }
    if (m_gravityEnabled) { 
        orb.velocity.y += gravity * deltaTime;
        orb.position += orb.velocity * deltaTime;
    }

    if (orb.position.y < 0.0f) {
        float impactVelocity = glm::abs(orb.velocity.y);

        //0.049 is a magic no remember that, say if y vel is 10, multiplying it by 0.1 gives 1.0 of +glow to ball
        float energyGain = impactVelocity * 0.049f;

        orb.position.y = 0.0f;
        orb.velocity.y = -orb.velocity.y * 0.98f; 
        orb.energy += energyGain; 
        orb.energy = glm::min(orb.energy, 1.0f);

        if (glm::abs(orb.velocity.y) < 0.1f) {
            orb.velocity.y = 0.0f;
            orb.energy -= energyGain; 
            orb.energy = glm::max(orb.energy, 0.0f); 
        }
    }
    
}

