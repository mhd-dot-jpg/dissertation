#version 460 core

in vec3 vertexPosition;
in float intensity;

uniform vec3 lightColor;

layout (location = 0) out vec4 lpvSH;

vec4 shCoefficients(vec3 n){
    return vec4(
        0.8862269,
        -1.0233267 * n.y,
        1.0233267 * n.z,
        -1.0233267 * n.x
    );
}

void main(){

    //float coefficients = 2.0f* 0.282094792f;
    float coefficients = 0.8862269f;
    float lpvSHR = lightColor.r*intensity*coefficients;
    float lpvSHG = lightColor.g*intensity*coefficients;
    float lpvSHB = lightColor.b*intensity*coefficients;
    lpvSH = vec4(lpvSHR, lpvSHG, lpvSHB, 1.0f);

}