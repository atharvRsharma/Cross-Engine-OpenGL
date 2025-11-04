#pragma once

#include <glad/glad.h>
#include "FileParser.h" 
#include <string>
#include <vector>
#include <cstddef>
#include <iostream> 

class Mesh
{
public:
    Mesh(const std::string& modelPath)
    {
        std::vector<Primitives::Vertex> vertices;
        parseObj(modelPath, vertices);

        if (vertices.empty()) {
            std::cerr << "ERROR::MESH: Failed to load model or model is empty: " << modelPath << std::endl;
            m_vertexCount = 0;
            m_vao = 0;
            m_vbo = 0;
            return;
        }

        m_vertexCount = static_cast<unsigned int>(vertices.size());

        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER,
            vertices.size() * sizeof(Primitives::Vertex),
            vertices.data(),
            GL_STATIC_DRAW);

        //pos
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Primitives::Vertex), (void*)offsetof(Primitives::Vertex, position));
        glEnableVertexAttribArray(0);
        //tex cooridnates
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Primitives::Vertex), (void*)offsetof(Primitives::Vertex, texCoord));
        glEnableVertexAttribArray(1);
        //normals
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Primitives::Vertex), (void*)offsetof(Primitives::Vertex, normal));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    ~Mesh()
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
    }

    void bind() const {
        glBindVertexArray(m_vao);
    }

    void unbind() const {
        glBindVertexArray(0);
    }

    unsigned int getVertexCount() const {
        return m_vertexCount;
    }

private:
    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    unsigned int m_vertexCount = 0;
};