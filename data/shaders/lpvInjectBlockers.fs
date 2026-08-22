#version 460 core

in vec3 vertexPosition;
in vec3 sPos;

uniform sampler2D normalTexture;
uniform sampler2D diffuseTexture;

layout (location = 0) out vec4 gvSH;

vec4 shCoefficients(vec3 n){
    return vec4(
        0.282094792,
        -0.488602512 * n.y,
        0.488602512 * n.z,
        -0.488602512 * n.x
    );
}

void main(){

    vec3 vplNormal = texture( normalTexture, vertexPosition.xy ).xyz;

    vec4 coefficients = shCoefficients(vplNormal);
    gvSH = coefficients;

}