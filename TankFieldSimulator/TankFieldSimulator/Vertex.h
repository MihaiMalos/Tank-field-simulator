#pragma once
#include <glm.hpp>

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    Vertex() = default;

    Vertex(float x, float y, float z, float n1, float n2, float n3, float tx, float ty)
        : Position(x, y, z), Normal(n1, n2, n3), TexCoords(tx, ty) 
    {
        // Empty
    }

    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};