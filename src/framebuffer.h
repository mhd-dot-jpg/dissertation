#pragma once

#include <map>

#include <glad/glad.h> 
#include "glm/glm.hpp"

class FrameBuffer{
public:
    FrameBuffer();
    FrameBuffer( glm::ivec2 dimensionss );
    FrameBuffer( glm::ivec2 dimensions, int colorChannels );
    FrameBuffer( glm::ivec3 dimensions, int colorChannels );
    void bind();
    GLuint depthTexture();
    GLuint colorTexture( int i );
private:
    glm::ivec3 dimensions;
    GLuint buffer;
    int numColorChannels;
    std::map<GLenum, GLuint> textures;
    glm::vec4 clearColor;
    bool depthTest;
};
