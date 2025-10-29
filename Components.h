#pragma once

#include <glm/glm.hpp>
#include <string>


struct GlowingOrb {
    std::string id = "entity_01";
    glm::vec3 position = glm::vec3(0.0f, 5.0f, 0.0f);
    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    float energy = 0.5f;
    std::string state = "active";
};