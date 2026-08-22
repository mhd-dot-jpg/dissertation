#include "glm/gtc/matrix_transform.hpp"

#include "light.h"

SpotLight::SpotLight( glm::vec3 position, glm::vec3 rotation, float angle ){

    this->position = position;
    this->rotation = rotation;
    projection = glm::perspective( glm::radians( angle ), 1.0f, 1.0f, 30.0f );
    color = glm::vec3( 1.0f );
    strength = 1.0f;

}

glm::mat4 SpotLight::viewTransform(){

    glm::mat4 yRotation = glm::rotate( glm::mat4(1.0f), glm::radians( rotation.y ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
    glm::mat4 rotationMatrix = glm::rotate( yRotation, glm::radians( rotation.x ), glm::vec3(1.0f, 0.0f, 0.0f) );
    glm::mat4 translation = glm::translate( glm::mat4( 1.0f ), position );
    glm::mat4 transform = translation * rotationMatrix;
    return glm::inverse(transform);

}

glm::mat4 SpotLight::projectionTransform(){
    return projection;
}

glm::vec3 SpotLight::direction(){

    glm::mat4 yRotation = glm::rotate( glm::mat4(1.0f), glm::radians( rotation.y ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
    glm::mat4 rotationMatrix = glm::rotate( yRotation, glm::radians( rotation.x ), glm::vec3(1.0f, 0.0f, 0.0f) );
    return glm::vec3(rotationMatrix * glm::vec4( 0.0f, 0.0f, -1.0f, 1.0f ));

}
