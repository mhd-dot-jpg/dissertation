#version 460 core

in vec3 vertexPosition;
in vec3 sPos;

uniform sampler2D normalTexture;
uniform sampler2D diffuseTexture;

layout (location = 0) out vec4 lpvSHR;
layout (location = 1) out vec4 lpvSHG;
layout (location = 2) out vec4 lpvSHB;

vec4 shCoefficients(vec3 n){
    return vec4(
        0.8862269,
        -1.0233267 * n.y,
        1.0233267 * n.z,
        -1.0233267 * n.x
    );
}

void main(){

    vec3 vplNormal = texture( normalTexture, vertexPosition.xy ).xyz;
    vec3 vplFlux = texture( diffuseTexture, vertexPosition.xy ).xyz;

    vec4 coefficients = shCoefficients(vplNormal);
    lpvSHR = vplFlux.r * coefficients;
    lpvSHG = vplFlux.g * coefficients;
    lpvSHB = vplFlux.b * coefficients;

}