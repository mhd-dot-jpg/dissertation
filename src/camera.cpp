#include "glm/gtc/matrix_transform.hpp"

#include "camera.h"
#include "input.h"

Camera::Camera( float fov, float aspectRatio, float nearClipDistance, float farClipDistance ){
    position = glm::vec3( 0.0f );
    rotation = glm::vec3( 0.0f );
    projection = glm::perspective( glm::radians( fov ), aspectRatio, nearClipDistance, farClipDistance );
}

glm::mat4 Camera::viewTransform(){

    glm::mat4 yRotation = glm::rotate( glm::mat4(1.0f), glm::radians( rotation.y ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
    glm::mat4 rotationMatrix = glm::rotate( yRotation, glm::radians( rotation.x ), glm::vec3(1.0f, 0.0f, 0.0f) );
    glm::mat4 translation = glm::translate( glm::mat4( 1.0f ), position );
    glm::mat4 transform = translation * rotationMatrix;
    return glm::inverse(transform);

}

glm::mat4 Camera::projectionTransform(){
    return projection;
}

void Camera::update( float dt ){

    if(mouseButtonDown( GLFW_MOUSE_BUTTON_RIGHT )){
        rotation.x -= 0.5f * getMouseVerticalMovement();
        rotation.y -= 0.5f * getMouseHorizontalMovement();
    }

    rotation.x = glm::clamp( rotation.x, -60.0f, 60.0f );

    glm::mat4 yRotation = glm::rotate( glm::mat4(1.0f), glm::radians( rotation.y ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
    glm::mat4 cameraRotation = glm::rotate( yRotation, glm::radians( rotation.x ), glm::vec3(1.0f, 0.0f, 0.0f) );

    glm::vec3 cameraMovement = glm::vec3(
        (float)(getKey( GLFW_KEY_D ) - getKey( GLFW_KEY_A )),
        0.0f,
        (float)(getKey(GLFW_KEY_S) - getKey(GLFW_KEY_W))
    );

    cameraMovement = (cameraRotation * glm::vec4(cameraMovement, 1.0f));
    
    cameraMovement += glm::vec3(
        0.0f,
        ((float)(getKey( GLFW_KEY_SPACE ) - getKey( GLFW_KEY_LEFT_CONTROL ))),
        0.0f
    );

    position += cameraMovement * dt;
}
