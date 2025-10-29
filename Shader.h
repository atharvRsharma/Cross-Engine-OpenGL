#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader
{
public:
    // The program ID
    unsigned int ID = 0;

    // Constructor: Reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    // Destructor
    ~Shader();

    // Activates the shader
    void use() const;

    // Utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    // Checks for compile or link errors
    void checkCompileErrors(unsigned int shader, std::string type);
};