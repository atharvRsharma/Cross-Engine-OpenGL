#include "FileParser.h"



void parseObj(const std::string& filePath, std::vector<Primitives::Vertex>& out_vertices)
{
    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec2> temp_tex_coords;
    std::vector<glm::vec3> temp_normals;

    out_vertices.clear();


    std::ifstream fileStream(filePath);

    if (!fileStream.is_open()) {
        std::cerr << "file path err: " << filePath << std::endl;

    }

    std::string line;
    while (std::getline(fileStream, line)) {


        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 position;
            ss >> position.x >> position.y >> position.z;
            temp_positions.push_back(position);
        }
        else if (prefix == "vt") {
            glm::vec2 tex_coords;
            ss >> tex_coords.x >> tex_coords.y;
            temp_tex_coords.push_back(tex_coords);
        }
        else if (prefix == "vn") {
            glm::vec3 normals;
            ss >> normals.x >> normals.y >> normals.z;
            temp_normals.push_back(normals);
        }
        else if (prefix == "f") {
            unsigned int vIndex1, vtIndex1, vnIndex1;
            unsigned int vIndex2, vtIndex2, vnIndex2;
            unsigned int vIndex3, vtIndex3, vnIndex3;
            char slash;
            Primitives::Vertex vertex1;
            Primitives::Vertex vertex2;
            Primitives::Vertex vertex3;

            ss >> vIndex1 >> slash >> vtIndex1 >> slash >> vnIndex1;
            ss >> vIndex2 >> slash >> vtIndex2 >> slash >> vnIndex2;
            ss >> vIndex3 >> slash >> vtIndex3 >> slash >> vnIndex3;

            vertex1.position = temp_positions[vIndex1 - 1];
            vertex1.texCoord = temp_tex_coords[vtIndex1 - 1];
            vertex1.normal = temp_normals[vnIndex1 - 1];
            out_vertices.push_back(vertex1);
            vertex2.position = temp_positions[vIndex2 - 1];
            vertex2.texCoord = temp_tex_coords[vtIndex2 - 1];
            vertex2.normal = temp_normals[vnIndex2 - 1];
            out_vertices.push_back(vertex2);
            vertex3.position = temp_positions[vIndex3 - 1];
            vertex3.texCoord = temp_tex_coords[vtIndex3 - 1];
            vertex3.normal = temp_normals[vnIndex3 - 1];
            out_vertices.push_back(vertex3);
        }

    }

    fileStream.close();

}