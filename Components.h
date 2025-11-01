#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>


enum class ShapeType {
    SPHERE,
    PLANE,
    AABB,
    NONE
};

enum class EquilibriumState {
    AWAKE,
    SLEEPING
};

struct Collider {
    ShapeType type;

    
    float radius;

    
    glm::vec3 normal;

    
    glm::vec3 baseHalfExtents = glm::vec3(0.5f);
};





struct GlowingOrb {
    std::string id = "entity_01";
    glm::vec3 position = glm::vec3(0.0f, 5.0f, 0.0f);
    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    float energy = 0.5f;
    std::string state = "active";
    Collider collider;
    EquilibriumState eq_state = EquilibriumState::AWAKE;
    
    float sleepTimer = 0.0f;

    void init() {
        position = glm::vec3(0.0f, 5.0f, 0.0f);
        

        collider.type = ShapeType::SPHERE;
        collider.radius = 1.0f; 
    }
};

struct Plane {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    Collider collider;


    void init() {
        collider.type = ShapeType::PLANE;
        collider.normal = glm::vec3(0.0f, 1.0f, 0.0f); //a plane at y=0 (facing up)
    }
};

struct Cube {
    glm::vec3 position = glm::vec3(2.0f);
    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 color = glm::vec3(1.0f, 0.0f, 1.0f);
    glm::vec3 scale = glm::vec3(1.0f);


    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); 
    glm::vec3 angularVelocity = glm::vec3(0.0f);
    glm::vec3 torque = glm::vec3(0.0f);

    float mass = 1.0f;
    float inverseMass = 1.0f/mass; 
    glm::mat3 inverseInertiaTensor;

    float sleepTimer = 0.0f;

    Collider collider;
    EquilibriumState state = EquilibriumState::AWAKE;

    void init() {
        collider.type = ShapeType::AABB;
        collider.baseHalfExtents = glm::vec3(1.0f); 

    }
};