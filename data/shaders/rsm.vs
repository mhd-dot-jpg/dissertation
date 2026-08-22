#version 460 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 2) in vec3 vTangent;
layout (location = 3) in vec3 vBitangent;
layout (location = 4) in vec2 vTexCoords;

out vec3 pos;
out vec3 norm;
out vec2 uv;
out float distance;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    pos = ( model * vec4( vPos, 1.0f ) ).xyz;
    norm = ( model * vec4( vNorm, 0.0f ) ).xyz;
    uv = vTexCoords;
    vec4 lightSpacePosition = view * model * vec4( vPos, 1.0f );
    distance = length( lightSpacePosition );
    gl_Position = projection * lightSpacePosition;
}