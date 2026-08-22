#pragma once

#include <string>
#include <map>

#include <glad/glad.h>
#include "glm/glm.hpp"

class Shader{
public:
    Shader();
    Shader( const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename );
    Shader( const std::string& vertexShaderFilename, const std::string& geometryFilename, const std::string& fragmentShaderFilename );
    Shader( const std::string& computeShaderFilename );
    void bind();
    void setUniform( const std::string& uniformName, GLuint value );
    void setUniform( const std::string& uniformName, int value );
    void setUniform( const std::string& uniformName, float value );
    void setUniform( const std::string& uniformName, glm::vec3 value );
    void setUniform( const std::string& uniformName, glm::vec4 value );
    void setUniform( const std::string& uniformName, glm::ivec3 value );
    void setUniform( const std::string& uniformName, glm::mat3 value );
    void setUniform( const std::string& uniformName, glm::mat4 value );
    void setTextureUniform( const std::string& uniformName, GLuint textureIndex, GLuint textureBuffer );
    GLuint program;
private:
    GLuint getUniformLocation( const std::string& uniformName );
    std::map<std::string, GLuint> uniformLocations;
};
