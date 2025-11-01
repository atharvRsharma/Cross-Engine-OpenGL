#include "Physics.h"
#include <glm/glm.hpp>
#include <algorithm> // For std::min/max



// The Main "Physics Loop"
void Physics::update(GlowingOrb& orb, Plane& plane, Cube* cube, const InputState& input, float deltaTime)
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
        const float gravity = -9.8f;

        if (orb.eq_state == EquilibriumState::AWAKE) {
            orb.velocity.y += gravity * deltaTime;
        }
        
       
    }

    if (orb.eq_state == EquilibriumState::AWAKE) {
        orb.position += orb.velocity * deltaTime;
    }
    


    solveSpherePlaneCollision(orb, plane);
    

    if (cube != nullptr) 
    {
        if (m_gravityEnabled) {
            if (cube->state == EquilibriumState::AWAKE) {
                cube->velocity.y += -9.8f * deltaTime;
            }
        }

        if (cube->state == EquilibriumState::AWAKE) {
            cube->position += cube->velocity * deltaTime;
        }

        
        solveCubePlaneCollision(*cube, plane); 
    }


}

void Physics::solveSpherePlaneCollision(GlowingOrb& orb, Plane& plane)
{
    float radius = orb.collider.radius;
    glm::vec3 normal = plane.collider.normal;
    float planeDist = glm::dot(plane.position, normal);
    float orbDist = glm::dot(orb.position, normal);
    float penetration = (orbDist - radius) - planeDist;


    if (penetration < 0.0f)
    {
        orb.eq_state = EquilibriumState::AWAKE;
        orb.sleepTimer = 0.0f;

        //resolving penetration(overlap)
        orb.position -= normal * penetration;

        //resolving vel
        float velocityAlongNormal = glm::dot(orb.velocity, normal);

        if (velocityAlongNormal < 0.0f) 
        {
            float impactVelocity = glm::abs(velocityAlongNormal);

            
            const float restThreshold = 0.1f; 

            if (impactVelocity < restThreshold)
            {
                orb.velocity = glm::vec3(0.0f);
                //avoid micro bounces, just set it to 0
            }
            else
            {
                //if impact is significant, like should bounce under regular

                float bounceFactor = 0.6f;
                glm::vec3 normalVelocity = normal * velocityAlongNormal;
                glm::vec3 bounceVelocity = -normalVelocity * bounceFactor;
                orb.velocity = orb.velocity - normalVelocity + bounceVelocity;

                // Handle Bounce Mechanics (Energy)
                float energyGain = impactVelocity * 0.1f; // Tweak this
                orb.energy += energyGain;
                orb.energy = glm::min(orb.energy, 1.0f);
            }
        }
    }
}

void Physics::solveCubePlaneCollision(Cube& cube, Plane& plane) {


    //collider data
    glm::vec3 halfExtents = cube.collider.baseHalfExtents * cube.scale;
    glm::vec3 normal = plane.collider.normal;


    float planeDist = glm::dot(plane.position, normal);
    float cubeCenterDist = glm::dot(cube.position, normal);

    //calculate cube's proj(no idea what dat means)
    float radius = halfExtents.x * abs(normal.x) + halfExtents.y * abs(normal.y) + halfExtents.z * abs(normal.z);

    //height of bottom face of cube(dist bw it and plane)
    float cubeBottomDist = cubeCenterDist - radius;

    if (cubeBottomDist < planeDist) {

        cube.state = EquilibriumState::AWAKE;
        cube.sleepTimer = 0.0f;

        float penetrationDepth = planeDist - cubeBottomDist;
        cube.position += normal * penetrationDepth;

        // 7. Resolve Velocity (Bounce)
        // *** THIS IS THE FIX ***
        // The velocity logic is now INSIDE the collision check

        float velocityAlongNormal = glm::dot(cube.velocity, normal);

        if (velocityAlongNormal < 0.0f) // Only if moving *into* the plane
        {
            float bounceFactor = 0.6f;

            // Calculate and apply the bounce
            glm::vec3 normalVelocity = normal * velocityAlongNormal;
            glm::vec3 bounceVelocity = -normalVelocity * bounceFactor;
            cube.velocity = cube.velocity - normalVelocity + bounceVelocity;
        }
    }
}