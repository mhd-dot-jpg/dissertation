#pragma once

#include "glm/glm.hpp"

class Camera{
public:
    Camera( float fov, float aspectRatio, float nearClipDistance, float farClipDistance );
    glm::mat4 viewTransform();
    glm::mat4 projectionTransform();
    void update( float dt );
    glm::mat4 projection;
    glm::vec3 position;
    glm::vec3 rotation;
};