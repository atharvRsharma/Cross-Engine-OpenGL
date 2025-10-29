#pragma once

#include "Components.h"
#include <string>

// Handles saving and loading the entity state to/from JSON
class Serializer
{
public:
    GlowingOrb loadState(const std::string& filePath);
    void saveState(const GlowingOrb& orb, const std::string& filePath);
};