#include <vector>

#include <glad/glad.h>
#include "glm/gtc/constants.hpp"

#include "lpv.h"
#include "shader.h"

GLuint injectionPointsVAO;
GLuint transferFunctionsBuffer;
GLuint gammaBuffer;
FrameBuffer singleScatteredBuffer;
FrameBuffer lpvPropagationBuffers[2];
FrameBuffer geometryVolumeBuffer;
int numInjectionPoints;
int steps;

Shader blockerInjectionShader;
Shader reflectedLightInjectionShader;
Shader scatteredLightInjectionShader;
Shader propagationShader;
Shader accumulationShader;

glm::ivec3 lpvResolution;
glm::vec3 lpvSize;

void generateInjectionPoints( glm::ivec2 resolution ){

    float xStep = 1.0f / resolution.x;
    float yStep = 1.0f / resolution.y;

    std::vector<glm::vec3> points;
    for(int j = 0; j < resolution.y; j++){
        for(int i = 0; i < resolution.x; i++){
            points.push_back( glm::vec3( xStep/2.0f + i * xStep, yStep/2.0f + j * yStep, 0.0f ) );
        }
    }

    GLuint injectionPoitnsVBO;

    glGenVertexArrays( 1, &injectionPointsVAO );
    glBindVertexArray( injectionPointsVAO );

    glGenBuffers( 1, &injectionPoitnsVBO );
    glBindBuffer( GL_ARRAY_BUFFER, injectionPointsVAO );

    glBufferData( GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, glm::vec3::length(), GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0 );

}

void generateTransferFunctions(){

    //glm::vec4 shCoeff = glm::vec4( 0.8862269f, -1.0233267f, 1.0233267f, -1.0233267f );
    glm::vec4 shCoeff = glm::vec4( 0.282094792, -0.488602512, 0.488602512, -0.488602512 );
    glm::vec4 transferFunctions[26];
    float gammas[26];

    glm::vec3 faceNormals[6] = {
        {-1.0f,  0.0f,  0.0f},
        { 1.0f,  0.0f,  0.0f},
        { 0.0f, -1.0f,  0.0f},
        { 0.0f,  1.0f,  0.0f},
        { 0.0f,  0.0f, -1.0f},
        { 0.0f,  0.0f,  1.0f},
    };

    int neighbourIndex = 0;
    float totalGamma = 0.0f;
    for(int i = -1; i <= 1; i++){
        for(int j = -1; j <= 1; j++){
            for(int k = -1; k <= 1; k++){
                if(!(i == 0 && j == 0 && k == 0)){
                    glm::vec3 nd = glm::normalize( glm::vec3((float)i,(float)j,(float)k) );
                    glm::vec4 transferFunction = glm::vec4( 0.0f );
                    for(int face = 0; face < 6; face++){
                        float faceOrientation = glm::dot( nd, faceNormals[face] );
                        if(faceOrientation >= 0.0f){
                            glm::vec3 directionToFace = glm::normalize( - nd - 0.5f * faceNormals[face]);
                            float solidAngle = 1.0f*glm::mix( 0.4006696844f / glm::pi<float>(), 0.423431354f / glm::pi<float>(), faceOrientation );
                            transferFunction += solidAngle * shCoeff * glm::vec4( 1.0f, directionToFace.y, directionToFace.z, directionToFace.x );
                        }
                    }
                    transferFunctions[neighbourIndex] = transferFunction;
                    float gamma = transferFunction.r * (2 * glm::sqrt( glm::pi<float>() ));
                    gammas[neighbourIndex] = gamma;
                    totalGamma += gamma;
                    neighbourIndex++;
                }
            }
        }
    }

    for(int i = 0; i < 26; i++){
        gammas[i] /= totalGamma;
    }

    glGenBuffers( 1, &transferFunctionsBuffer );
    glGenBuffers( 1, &gammaBuffer );
    glBindBuffer( GL_SHADER_STORAGE_BUFFER, transferFunctionsBuffer );
    glBufferData( GL_SHADER_STORAGE_BUFFER, sizeof(transferFunctions), transferFunctions, GL_STATIC_DRAW );
    glBindBuffer( GL_SHADER_STORAGE_BUFFER, gammaBuffer );
    glBufferData( GL_SHADER_STORAGE_BUFFER, sizeof(gammas), gammas, GL_STATIC_DRAW );
    glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 );

}

void initLPV( glm::ivec3 resolution, glm::vec3 size, glm::ivec2 injectionResolution, int propagationSteps ){

    lpvResolution = resolution;
    lpvSize = size;
    numInjectionPoints = injectionResolution.x * injectionResolution.y;
    generateInjectionPoints( injectionResolution );
    generateTransferFunctions();
    singleScatteredBuffer = FrameBuffer(resolution, 1);
    lpvPropagationBuffers[0] = FrameBuffer(resolution, 3);
    lpvPropagationBuffers[1] = FrameBuffer(resolution, 3);
    geometryVolumeBuffer = FrameBuffer(resolution, 1);
    steps = propagationSteps;
    blockerInjectionShader = Shader( "shaders/lpvInjectBlockers.vs", "shaders/lpvInjectBlockers.gs", "shaders/lpvInjectBlockers.fs" );
    reflectedLightInjectionShader = Shader( "shaders/lpvInjectReflected.vs", "shaders/lpvInjectReflected.gs", "shaders/lpvInjectReflected.fs" );
    scatteredLightInjectionShader = Shader( "shaders/lpvInjectScattered.vs", "shaders/lpvInjectScattered.gs", "shaders/lpvInjectScattered.fs" );
    propagationShader = Shader( "shaders/lpvPropagate.cs" );
    accumulationShader = Shader( "shaders/lpvAccumulate.cs" );

}

void injectBlockers( Camera& camera, FrameBuffer& gBuffer, SpotLight& light, FrameBuffer& rsmBuffer ){

    geometryVolumeBuffer.bind();
    blockerInjectionShader.bind();
    blockerInjectionShader.setUniform( "normalTexture", 0 );
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, rsmBuffer.colorTexture( 1 ) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    blockerInjectionShader.setUniform( "positionTexture", 1 );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, rsmBuffer.colorTexture( 2 ) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    blockerInjectionShader.setUniform( "lpvDimensions", lpvResolution );
    blockerInjectionShader.setUniform( "lpvScale", lpvSize );
    glDrawArrays( GL_POINTS, 0, numInjectionPoints );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, gBuffer.colorTexture( 1 ) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    blockerInjectionShader.setUniform( "positionTexture", 1 );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, gBuffer.colorTexture( 2 ) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glDrawArrays( GL_POINTS, 0, numInjectionPoints );


}

void injectReflectedLight( FrameBuffer lpvBuffer, FrameBuffer rsmBuffer, SpotLight light, float absorption, float scattering ){

    lpvBuffer.bind();
    glBindVertexArray( injectionPointsVAO );
    reflectedLightInjectionShader.bind();
    reflectedLightInjectionShader.setUniform( "diffuseTexture", 0 );
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, rsmBuffer.colorTexture( 0 ) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    reflectedLightInjectionShader.setUniform( "normalTexture", 1 );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, rsmBuffer.colorTexture( 1 ) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    reflectedLightInjectionShader.setUniform( "positionTexture", 2 );
    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, rsmBuffer.colorTexture( 2 ) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    reflectedLightInjectionShader.setUniform( "lpvDimensions", lpvResolution );
    reflectedLightInjectionShader.setUniform( "lpvScale", lpvSize );
    reflectedLightInjectionShader.setUniform( "absorption", absorption );
    reflectedLightInjectionShader.setUniform( "scattering", scattering );
    glDrawArrays( GL_POINTS, 0, numInjectionPoints );

    singleScatteredBuffer.bind();
    scatteredLightInjectionShader.bind();
    scatteredLightInjectionShader.setUniform( "positionTexture", 0 );
    scatteredLightInjectionShader.setUniform( "spotLightDirection", light.direction() );
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, rsmBuffer.colorTexture( 2 ) );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    scatteredLightInjectionShader.setUniform( "lpvDimensions", lpvResolution );
    scatteredLightInjectionShader.setUniform( "lpvScale", lpvSize );
    scatteredLightInjectionShader.setUniform( "lightPosition", light.position );
    scatteredLightInjectionShader.setUniform( "lightColor", light.color * light.strength );
    scatteredLightInjectionShader.setUniform( "absorption", absorption );
    scatteredLightInjectionShader.setUniform( "scattering", scattering );
    glDrawArrays( GL_POINTS, 0, numInjectionPoints );

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

}

void setAccumulationBuffers( FrameBuffer sourceBuffer, FrameBuffer destinationBuffer ){

    glBindImageTexture( 0, sourceBuffer.colorTexture(0), 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F );
    glBindImageTexture( 1, sourceBuffer.colorTexture(1), 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F );
    glBindImageTexture( 2, sourceBuffer.colorTexture(2), 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F );
    glBindImageTexture( 3, destinationBuffer.colorTexture(0), 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F );
    glBindImageTexture( 4, destinationBuffer.colorTexture(1), 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F );
    glBindImageTexture( 5, destinationBuffer.colorTexture(2), 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F );

}

void propagateLight( FrameBuffer lpvBuffer, float occlusionStrength, float absorption, float scattering ){

    lpvPropagationBuffers[0].bind();
    lpvPropagationBuffers[1].bind();

    accumulationShader.bind();
    setAccumulationBuffers( lpvBuffer, lpvPropagationBuffers[0] );
    glDispatchCompute( lpvResolution.x, lpvResolution.y, lpvResolution.z );

    for(int i = 0; i < steps; i++){

        propagationShader.bind();
        propagationShader.setUniform( "step", i );
        propagationShader.setUniform( "lpvDimensions", lpvResolution );
        propagationShader.setUniform( "lpvScale", lpvSize );
        propagationShader.setUniform( "occlusionStrength", occlusionStrength );
        propagationShader.setUniform( "singleScatteredSH", 0 );
        propagationShader.setUniform( "absorption", absorption );
        propagationShader.setUniform( "scattering", scattering );
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_3D, singleScatteredBuffer.colorTexture( 0 ) );
        propagationShader.setUniform( "gvSH", 1 );
        glActiveTexture( GL_TEXTURE1 );
        glBindTexture( GL_TEXTURE_3D, geometryVolumeBuffer.colorTexture( 0 ) );

        int flip = (i%2 == 0)?0:1;
        setAccumulationBuffers( lpvPropagationBuffers[flip], lpvPropagationBuffers[1 - flip] );

        glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 6, transferFunctionsBuffer );
        glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 7, gammaBuffer );
        glDispatchCompute( lpvResolution.x, lpvResolution.y, lpvResolution.z );

        accumulationShader.bind();
        setAccumulationBuffers( lpvPropagationBuffers[1-flip], lpvBuffer );
        glDispatchCompute( lpvResolution.x, lpvResolution.y, lpvResolution.z );

    }

}
