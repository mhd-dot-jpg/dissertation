#include "renderer.h"
#include "glm/gtc/type_ptr.hpp"

GLuint screenQuadVAO;
Shader renderTextureShader;
Shader raymarchShader;

void initRenderer(){

    GLuint screenQuadVBO;
    float screenQuadVertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // Top Left
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // Bottom Right
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // Bottom Left
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // Top Left
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // Top Right
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // Bottom Right
    };


    glGenVertexArrays( 1, &screenQuadVAO );
    glBindVertexArray( screenQuadVAO );

    glGenBuffers( 1, &screenQuadVBO );
    glBindBuffer( GL_ARRAY_BUFFER, screenQuadVBO );

    glBufferData( GL_ARRAY_BUFFER, sizeof(screenQuadVertices), screenQuadVertices, GL_STATIC_DRAW );

    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, glm::vec3::length(), GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0 );
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 1, glm::vec2::length(), GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)) );

    renderTextureShader = Shader( "shaders/renderTexture.vs", "shaders/renderTexture.fs" );
    raymarchShader = Shader( "shaders/raymarch.vs", "shaders/raymarch.fs" );

    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

}

void renderModel( const Model& model ){

    int numMeshes = model.meshes.size();
    const Mesh* meshes = model.meshes.data();
    const GLuint* materials = model.materials.data();

    for(int i = 0; i < numMeshes; i++){

        GLuint textureID = materials[meshes[i].materialIndex];

        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, textureID );
        glBindVertexArray( meshes[i].vao );
        glDrawElements( GL_TRIANGLES, meshes[i].indices.size(), GL_UNSIGNED_INT, 0 );

    }

}


void render( std::vector<Model>& objects, Camera& camera, Shader& shader, FrameBuffer& framebuffer ){

    framebuffer.bind();
    shader.bind();
    shader.setUniform( "projection", camera.projectionTransform() );
    shader.setUniform( "view", camera.viewTransform() );
    shader.setUniform( "diffuseTexture", 0 );
    for(Model model : objects){
        shader.setUniform( "model", model.transform );
        renderModel( model );
    }
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

}

void renderGBuffer( FrameBuffer& screenBuffer, FrameBuffer& gBuffer, Camera& cam, SpotLight& light, FrameBuffer& shadowMap,
                    FrameBuffer& lpv, glm::ivec3 lpvResolution, glm::vec3 lpvSize, FrameBuffer& raymarchBuffer,
                    float absorption, float scattering, float lpvStrength ){

    screenBuffer.bind();
    renderTextureShader.bind();
    renderTextureShader.setUniform( "diffuseTexture", 0 );
    renderTextureShader.setUniform( "normalTexture", 1 );
    renderTextureShader.setUniform( "positionTexture", 2 );
    renderTextureShader.setUniform( "lightPosition", light.position );
    renderTextureShader.setUniform( "lightColor", light.color * light.strength );
    renderTextureShader.setUniform( "spotLightDirection", light.direction() );
    renderTextureShader.setUniform( "lightProjectionMatrix", light.projectionTransform() );
    renderTextureShader.setUniform( "lightViewMatrix", light.viewTransform() );
    renderTextureShader.setUniform( "lightDepthTexture", 3 );
    renderTextureShader.setUniform( "lpv.r", 4 );
    renderTextureShader.setUniform( "lpv.g", 5 );
    renderTextureShader.setUniform( "lpv.b", 6 );
    renderTextureShader.setUniform( "lpv.resolution", lpvResolution );
    renderTextureShader.setUniform( "lpv.scale", lpvSize );
    renderTextureShader.setUniform( "raymarchResult", 7 );
    renderTextureShader.setUniform( "absorption", absorption );
    renderTextureShader.setUniform( "scattering", scattering );
    renderTextureShader.setUniform( "cameraPosition", cam.position );
    renderTextureShader.setUniform( "lpvStrength", lpvStrength );
    glBindVertexArray( screenQuadVAO );
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, gBuffer.colorTexture( 0 ) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, gBuffer.colorTexture( 1 ) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, gBuffer.colorTexture( 2 ) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glActiveTexture( GL_TEXTURE3 );
    glBindTexture( GL_TEXTURE_2D, shadowMap.depthTexture() );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glActiveTexture( GL_TEXTURE4 );
    glBindTexture( GL_TEXTURE_3D, lpv.colorTexture( 0 ) );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glActiveTexture( GL_TEXTURE5 );
    glBindTexture( GL_TEXTURE_3D, lpv.colorTexture( 1 ) );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glActiveTexture( GL_TEXTURE6 );
    glBindTexture( GL_TEXTURE_3D, lpv.colorTexture( 2 ) );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glActiveTexture( GL_TEXTURE7 );
    glBindTexture( GL_TEXTURE_2D, raymarchBuffer.colorTexture( 0 ) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glDrawArrays( GL_TRIANGLES, 0, 6 );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

}

void raymarchLPV( FrameBuffer& buffer, FrameBuffer& gBuffer, Camera& camera, FrameBuffer& lpv, glm::ivec3 lpvResolution, glm::vec3 lpvSize, float absorption, float scattering ){

    buffer.bind();
    raymarchShader.bind();
    raymarchShader.setUniform( "positionTexture", 0 );
    raymarchShader.setUniform( "cameraPosition", camera.position );
    raymarchShader.setUniform( "lpv.r", 1 );
    raymarchShader.setUniform( "lpv.g", 2 );
    raymarchShader.setUniform( "lpv.b", 3 );
    raymarchShader.setUniform( "lpv.resolution", lpvResolution );
    raymarchShader.setUniform( "lpv.scale", lpvSize );
    raymarchShader.setUniform( "absorption", absorption );
    raymarchShader.setUniform( "scattering", scattering );
    glBindVertexArray( screenQuadVAO );
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, gBuffer.colorTexture( 2 ) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_3D, lpv.colorTexture( 0 ) );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_3D, lpv.colorTexture( 1 ) );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glActiveTexture( GL_TEXTURE3 );
    glBindTexture( GL_TEXTURE_3D, lpv.colorTexture( 2 ) );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glDrawArrays( GL_TRIANGLES, 0, 6 );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

}

void renderShadowMap( std::vector<Model>& objects, SpotLight light, Shader& shader, FrameBuffer& framebuffer, float absorption, float scattering ){

    framebuffer.bind();
    shader.bind();
    shader.setUniform( "projection", light.projectionTransform() );
    shader.setUniform( "view", light.viewTransform() );
    shader.setUniform( "diffuseTexture", 0 );
    shader.setUniform( "lightPosition", light.position );
    shader.setUniform( "lightColor", light.color * light.strength );
    shader.setUniform( "spotLightDirection", light.direction() );
    shader.setUniform( "absorption", absorption );
    shader.setUniform( "scattering", scattering );
    for(Model model : objects){
        shader.setUniform( "model", model.transform );
        renderModel( model );
    }
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

}
