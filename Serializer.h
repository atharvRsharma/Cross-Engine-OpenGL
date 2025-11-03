#pragma once

#include "Components.h"
#include <string>

class Serializer
{
public:
    GlowingOrb loadState(const std::string& filePath);
    void saveState(const GlowingOrb& orb, const std::string& filePath);
};