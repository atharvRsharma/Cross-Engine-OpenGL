#include "Physics.h"
#include <glm/glm.hpp>
#include <algorithm> 



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

    orb.forceAccumulator = glm::vec3(0.0f);
    if (cube != nullptr) {
        cube->forceAccumulator = glm::vec3(0.0f);
    }


    if (m_gravityEnabled) {
        const float gravity = -9.8f;

        if (orb.eq_state == EquilibriumState::AWAKE) {
            glm::vec3 orbGravityForce = glm::vec3(0.0f, gravity * orb.mass, 0.0f);
            orb.forceAccumulator += orbGravityForce;
        }
        if (cube != nullptr && cube->state == EquilibriumState::AWAKE) {
            glm::vec3 cubeGravityForce = glm::vec3(0.0f, gravity * cube->mass, 0.0f);
            cube->forceAccumulator += cubeGravityForce;
        }
    }
        
       
    

    if (orb.eq_state == EquilibriumState::AWAKE) {
        // a = F * (1/m)
        glm::vec3 acceleration = orb.forceAccumulator * orb.inverseMass;
        // v_new = v_old + a * dt
        orb.velocity += acceleration * deltaTime;
        // p_new = p_old + v * dt
        orb.position += orb.velocity * deltaTime;
    }
    if (cube != nullptr && cube->state == EquilibriumState::AWAKE) {
        // a = F * (1/m)
        glm::vec3 acceleration = cube->forceAccumulator * cube->inverseMass;
        // v_new = v_old + a * dt
        cube->velocity += acceleration * deltaTime;
        // p_new = p_old + v * dt
        cube->position += cube->velocity * deltaTime;
    }
    


    solveSpherePlaneCollision(orb, plane);
    if (cube != nullptr) {
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

        //vel resolution
        float velocityAlongNormal = glm::dot(cube.velocity, normal);

        if (velocityAlongNormal < 0.0f) //object moving into the plane, like in the plane facing dir ig
        {
            float bounceFactor = 0.6f;

            //apply bounce
            glm::vec3 normalVelocity = normal * velocityAlongNormal;
            glm::vec3 bounceVelocity = -normalVelocity * bounceFactor;
            cube.velocity = cube.velocity - normalVelocity + bounceVelocity;
        }
    }
}