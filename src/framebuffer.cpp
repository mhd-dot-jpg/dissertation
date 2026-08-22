#include <iostream>

#include "framebuffer.h"

const GLuint COLOR_ATTACHMENTS[8] = {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3,
    GL_COLOR_ATTACHMENT4,
    GL_COLOR_ATTACHMENT5,
    GL_COLOR_ATTACHMENT6,
    GL_COLOR_ATTACHMENT7,
};

GLuint createTexture( int width, int height, GLenum internalFormat, GLenum format ){

    GLuint texture;

    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_FLOAT, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glBindTexture( GL_TEXTURE_2D, 0 );

    return texture;

}

GLuint createTexture3D( glm::ivec3 dimensions, GLenum internalFormat, GLenum format ){

    GLuint texture;

    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_3D, texture );
    glTexImage3D( GL_TEXTURE_3D, 0, internalFormat, dimensions.x, dimensions.y, dimensions.z, 0, format, GL_FLOAT, 0 );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

    return texture;

}

FrameBuffer::FrameBuffer(){
    dimensions = glm::ivec3( 0 );
    numColorChannels = 0;
    buffer = 0;
    clearColor = glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f );
    depthTest = true;
}

FrameBuffer::FrameBuffer( glm::ivec2 dimensions ){

    this->dimensions = glm::ivec3(dimensions, 1 );
    numColorChannels = 0;
    clearColor = glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f );
    depthTest = false;
    buffer = 0;

}

FrameBuffer::FrameBuffer( glm::ivec2 dimensions, int colorChannels ){

    this->dimensions = glm::ivec3(dimensions, 0 );
    numColorChannels = colorChannels;

    GLuint depthTexture;
    glGenFramebuffers( 1, &buffer );
    depthTexture = createTexture( dimensions.x, dimensions.y, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT );
    
    glBindFramebuffer( GL_FRAMEBUFFER, buffer );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0 );
    textures[GL_DEPTH_ATTACHMENT] = depthTexture;
    if(colorChannels > 0){

        for(int i = 0; i < colorChannels; i++){
            GLuint colorTexture = createTexture( dimensions.x, dimensions.y, GL_RGBA16F, GL_RGBA );
            glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorTexture, 0 );
            textures[GL_COLOR_ATTACHMENT0 + i] = colorTexture;
        }

    }
    else{

        glDrawBuffer( GL_NONE );
        glReadBuffer( GL_NONE );

    }

    if(glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE){

        std::cerr << "OpenGL Error: " << glGetError() << std::endl;

    }

    glBindFramebuffer( GL_FRAMEBUFFER, 0 ); 
    clearColor = glm::vec4( 0.0f );
    depthTest = true;

}

FrameBuffer::FrameBuffer( glm::ivec3 dimensions, int colorChannels ){

    this->dimensions = dimensions;
    numColorChannels = colorChannels;
    textures[GL_DEPTH_ATTACHMENT] = 0;

    glGenFramebuffers( 1, &buffer );
    
    glBindFramebuffer( GL_FRAMEBUFFER, buffer );
    if(colorChannels > 0){

        for(int i = 0; i < colorChannels; i++){
            GLuint colorTexture = createTexture3D( dimensions, GL_RGBA16F, GL_RGBA );
            glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, colorTexture, 0 );
            textures[GL_COLOR_ATTACHMENT0 + i] = colorTexture;
        }

    }
    else{

        glDrawBuffer( GL_NONE );
        glReadBuffer( GL_NONE );

    }

    if(glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE){

        std::cerr << "OpenGL Error: " << glGetError() << std::endl;

    }

    glBindFramebuffer( GL_FRAMEBUFFER, 0 ); 

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    clearColor = glm::vec4( 0.0f );
    depthTest = false;

}

void FrameBuffer::bind(){
    
    glClearColor( clearColor.r, clearColor.g, clearColor.b, clearColor.a );
    glBindFramebuffer( GL_FRAMEBUFFER, buffer );
    if(depthTest){
        glEnable( GL_DEPTH_TEST );
    } else {
        glDisable( GL_DEPTH_TEST );
    }
    glViewport( 0, 0, dimensions.x, dimensions.y );
    if(buffer != 0){
        glDrawBuffers( numColorChannels, COLOR_ATTACHMENTS );
    }
    if(depthTest){
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    } else {
        glClear( GL_COLOR_BUFFER_BIT );
    }

}

GLuint FrameBuffer::depthTexture(){

    return textures[GL_DEPTH_ATTACHMENT];

}

GLuint FrameBuffer::colorTexture( int i ){

    if(i >= numColorChannels){
        return -1;
    }

    return textures[GL_COLOR_ATTACHMENT0 + i];

}
