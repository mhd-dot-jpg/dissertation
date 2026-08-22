#version 460 core

in vec3 pos;
in vec3 norm;
in vec2 uv;

layout (location = 0) out vec4 diffuseOut;
layout (location = 1) out vec4 normalOut;
layout (location = 2) out vec4 positionOut;

uniform sampler2D diffuseTexture;

void main(){
    diffuseOut = texture( diffuseTexture, uv ); 
    normalOut = vec4( normalize( norm ), 1.0f );
    positionOut = vec4( pos, 1.0f );
}