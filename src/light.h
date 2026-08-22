#pragma once

#include "glm/glm.hpp"

class SpotLight{
public:
    SpotLight( glm::vec3 position, glm::vec3 rotation, float angle );
    glm::mat4 viewTransform();
    glm::mat4 projectionTransform();
    glm::vec3 direction();
    glm::mat4 projection;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 color;
    float strength;
};
