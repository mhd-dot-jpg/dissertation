#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

#include "shader.h"
#include "framebuffer.h"
#include "model.h"
#include "camera.h"
#include "input.h"
#include "renderer.h"
#include "light.h"
#include "lpv.h"
#include "interface.h"

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

const glm::ivec3 LPV_RESOLUTION = { 32, 32, 32 };
const glm::vec3 LPV_SIZE = { 15.0f, 15.0f, 15.0f };

GLFWwindow* initWindow(){

    glfwInit();
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    GLFWwindow* window = glfwCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, "Dissertation", NULL, NULL );
    if(window == NULL){
        std::cerr << "Error creating window." << std::endl;
        return NULL;
    }

    glfwMakeContextCurrent( window );
    glfwSwapInterval( 0 );
    
    setCallbacks( window );

    return window;

}

void initGLAD(){

    if(!gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress )){
        std::cerr << "Error initialising GLAD." << std::endl;
    }

}

int main(int arc, char **argv){

    GLFWwindow* window = initWindow();
    initGLAD();
    initRenderer();
    initUI( window );

    FrameBuffer screenBuffer( glm::ivec2(WINDOW_WIDTH, WINDOW_HEIGHT) );
    FrameBuffer lpvBuffer( LPV_RESOLUTION, 3);

    Shader gBufferShader( "shaders/gBuffer.vs", "shaders/gBuffer.fs" );
    FrameBuffer gBuffer( glm::ivec2(WINDOW_WIDTH, WINDOW_HEIGHT), 3 );

    SpotLight light( glm::vec3( 0.0f, 8.0f, 0.0f ), glm::vec3( -45.0f, 90.0f, 0.0f ), 90.0f );
    FrameBuffer rsmBuffer( glm::ivec2( 512, 512 ), 3 );
    Shader rsmShader( "shaders/rsm.vs", "shaders/rsm.fs" );
    FrameBuffer raymarchBuffer( glm::ivec2(WINDOW_WIDTH, WINDOW_HEIGHT) / 4, 1 );

    float aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
    Camera mainCam( 90.0f, aspectRatio, 0.1f, 30.0f );

    std::vector<Model> objects;
    objects.push_back( Model( "models/sponza.obj" ) );

    initLPV( LPV_RESOLUTION, LPV_SIZE, glm::ivec2( 512, 512 ), 8 );
    float occlusionStrength = 0.0f;
    float absorption = 0.0f;
    float scattering = 0.0f;
    float lpvStrength = 1.0f;

    while(!glfwWindowShouldClose( window )){

        startUIFrame();

        calcMouseMovement( window );

        mainCam.update( 0.1f );
        render( objects, mainCam, gBufferShader, gBuffer );
        renderShadowMap( objects, light, rsmShader, rsmBuffer, absorption, scattering );

        injectReflectedLight( lpvBuffer, rsmBuffer, light, absorption, scattering );
        injectBlockers( mainCam, gBuffer, light, rsmBuffer );
        propagateLight( lpvBuffer, occlusionStrength, absorption, scattering );

        raymarchLPV( raymarchBuffer, gBuffer, mainCam, lpvBuffer, LPV_RESOLUTION, LPV_SIZE, absorption, scattering );
        renderGBuffer( screenBuffer, gBuffer, mainCam, light, rsmBuffer, lpvBuffer, LPV_RESOLUTION, LPV_SIZE, raymarchBuffer, absorption, scattering, lpvStrength );

        renderUI( light, occlusionStrength, absorption, scattering, lpvStrength );

        glfwSwapBuffers( window );
        glfwPollEvents();

    }

    glfwTerminate();

    return 0;
}
