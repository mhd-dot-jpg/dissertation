#pragma once

#include <vector>

#include <assimp/scene.h>
#include <glad/glad.h>
#include "glm/glm.hpp"

struct Vertex{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec2 uv;
};

class Mesh{
public:
    Mesh( aiMesh* mesh );
    GLuint vao;
    unsigned int materialIndex;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
};

class Model{
public:
    Model( const std::string& filename );
    std::vector<Mesh> meshes;
    std::vector<GLuint> materials;
    glm::mat4 transform;
};
