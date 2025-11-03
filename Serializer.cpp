#include "Serializer.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

float cleanFloat(float value) {
    const float EPSILON = 1e-6f; 
    if (std::abs(value) < EPSILON) {
        return 0.0f;
    }
    return value;
}

GlowingOrb Serializer::loadState(const std::string& filePath) {
    GlowingOrb orb;
    std::ifstream f(filePath);
    orb.init();

    if (!f.is_open()) {
        std::cout << "INFO: No '" << filePath << "' found. Starting new simulation." << std::endl;
        return orb;
    }

    try {
        json j;
        f >> j;
        f.close();

        orb.id = j.at("id").get<std::string>();

        json pos_j = j.at("position");
        orb.position = glm::vec3(pos_j[0].get<float>(), pos_j[1].get<float>(), pos_j[2].get<float>());

        json vel_j = j.at("velocity");
        orb.velocity = glm::vec3(vel_j[0].get<float>(), vel_j[1].get<float>(), vel_j[2].get<float>());

        orb.energy = j.at("energy").get<float>();
        orb.state = j.at("state").get<std::string>();

        orb.isGravityOn = j.at("isGravityOn").get<bool>();

        std::cout << "SUCCESS: Loaded previous state from '" << filePath << "'." << std::endl;

    }
    catch (json::exception& e) {
        std::cerr << "ERROR: Failed to parse '" << filePath << "'. " << e.what() << std::endl;
        std::cerr << "Starting new simulation with default values." << std::endl;
    }
    return orb;
}

void Serializer::saveState(const GlowingOrb& orb, const std::string& filePath) {
    json j;


    glm::vec3 cleanPosition = {
        cleanFloat(orb.position.x),
        cleanFloat(orb.position.y),
        cleanFloat(orb.position.z)
    };

    glm::vec3 cleanVelocity = {
        cleanFloat(orb.velocity.x),
        cleanFloat(orb.velocity.y),
        cleanFloat(orb.velocity.z)
    };
 

    glm::vec3 savedPosition = orb.position;


    j["id"] = orb.id;
    j["position"] = { cleanPosition.x, cleanPosition.y, cleanPosition.z }; 
    j["velocity"] = { cleanVelocity.x, cleanVelocity.y, cleanVelocity.z }; 
    j["energy"] = orb.energy;
    j["state"] = orb.state;
    j["isGravityOn"] = orb.isGravityOn;

    try {
        std::ofstream o(filePath);
        o << j.dump(4);
        o.close();
        std::cout << "SUCCESS: Entity state saved to " << filePath << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: Failed to save state to JSON. " << e.what() << std::endl;
    }
}