#include <fstream>

#include "glm/gtc/type_ptr.hpp"

#include "shader.h"

GLuint compileShader( GLuint type, const std::string& filename ){
    
    std::ifstream shaderFile = std::ifstream( filename );

    if(!shaderFile){
        fprintf( stderr, "Could not open shader file %s\n", filename.c_str() );
        return 0;
    }

    std::string shaderSource( (std::istreambuf_iterator<char>( shaderFile )), (std::istreambuf_iterator<char>()) );

    GLuint shaderID = glCreateShader( type );
    
    if(shaderID == 0){
        fprintf( stderr, "Could not create shader %s\n", filename.c_str() );
        return 0;
    }

    const char* shaderSourceCopy = shaderSource.c_str();

    glShaderSource( shaderID, 1, (const GLchar **) &shaderSourceCopy, NULL );
    glCompileShader( shaderID );

    GLint success;
    glGetShaderiv( shaderID, GL_COMPILE_STATUS, &success );

    if(!success){
        GLchar log[1024];
        glGetShaderInfoLog( shaderID, 1024, NULL, log );
        fprintf( stderr, "Error compiling shader %s: %s\n", filename.c_str(), log);
        return 0;
    }

    printf( "Compiled shader %s\n", filename.c_str() );
    shaderFile.close();
    return shaderID;

}

GLuint createShaderProgram( const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader = "" ){
    
    GLuint program = glCreateProgram();
    GLuint vertexShaderID = compileShader( GL_VERTEX_SHADER, vertexShader );
    GLuint fragmentShaderID = compileShader( GL_FRAGMENT_SHADER, fragmentShader );

    glAttachShader( program, vertexShaderID );
    glAttachShader( program, fragmentShaderID );

    if(geometryShader.length() != 0){
        GLuint geometryShaderID = compileShader( GL_GEOMETRY_SHADER, geometryShader );
        glAttachShader( program, geometryShaderID );
    }

    glLinkProgram( program );

    glDeleteShader( vertexShaderID );
    glDeleteShader( fragmentShaderID );

    return program;

}

GLuint createComputeShaderProgram( const std::string& filename ){
    
    GLuint program = glCreateProgram();
    GLuint shader = compileShader( GL_COMPUTE_SHADER, filename );

    glAttachShader( program, shader );
    glLinkProgram( program );
    glDeleteShader( shader );

    return program;

}

Shader::Shader(){
    program = 0;
}

Shader::Shader( const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename ){
    program = createShaderProgram( vertexShaderFilename, fragmentShaderFilename );
}

Shader::Shader( const std::string& vertexShaderFilename, const std::string& geometryFilename, const std::string& fragmentShaderFilename ){
    program = createShaderProgram( vertexShaderFilename, fragmentShaderFilename, geometryFilename );
}

Shader::Shader( const std::string& computeShaderFilename ){
    program = createComputeShaderProgram( computeShaderFilename );
}

void Shader::bind(){
    glUseProgram( program );
}

void Shader::setUniform( const std::string& uniformName, GLuint value ){

    GLuint location = getUniformLocation( uniformName );
    glUniform1ui( location, value );

}

void Shader::setUniform( const std::string& uniformName, int value ){

    GLuint location = getUniformLocation( uniformName );
    glUniform1i( location, value );

}

void Shader::setUniform( const std::string& uniformName, float value ){

    GLuint location = getUniformLocation( uniformName );
    glUniform1f( location, value );

}

void Shader::setUniform( const std::string& uniformName, glm::vec3 value ){

    GLuint location = getUniformLocation( uniformName );
    glUniform3fv( location, 1, glm::value_ptr( value ) );

}

void Shader::setUniform( const std::string& uniformName, glm::vec4 value ){

    GLuint location = getUniformLocation( uniformName );
    glUniform4fv( location, 1, glm::value_ptr( value ) );

}

void Shader::setUniform( const std::string& uniformName, glm::ivec3 value){

    GLuint location = getUniformLocation( uniformName );
    glUniform3iv( location, 1, glm::value_ptr( value ) );

}

void Shader::setUniform( const std::string& uniformName, glm::mat3 value ){

    GLuint location = getUniformLocation( uniformName );
    glUniformMatrix3fv( location,1, GL_FALSE, glm::value_ptr( value ) );

}

void Shader::setUniform( const std::string& uniformName, glm::mat4 value ){

    GLuint location = getUniformLocation( uniformName );
    glUniformMatrix4fv( location,1, GL_FALSE, glm::value_ptr( value ) );

}

void Shader::setTextureUniform( const std::string& uniformName, GLuint textureIndex, GLuint textureBuffer ){
    setUniform( uniformName, textureIndex );
    glActiveTexture( GL_TEXTURE0+textureIndex );
    glBindTexture( GL_TEXTURE_2D, textureBuffer );
}

GLuint Shader::getUniformLocation( const std::string& uniformName ){

    if(uniformLocations.find( uniformName ) == uniformLocations.end()){
        GLuint location = glGetUniformLocation( program, uniformName.c_str() );
        uniformLocations[uniformName] = location;
        return location;
    }
        
    return uniformLocations[uniformName];

}

