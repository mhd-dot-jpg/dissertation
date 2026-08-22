#version 460 core

in vec3 pos;
in vec3 norm;
in vec2 uv;
in float distance;

layout (location = 0) out vec4 diffuseOut;
layout (location = 1) out vec4 normalOut;
layout (location = 2) out vec4 positionOut;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 spotLightDirection;
uniform sampler2D diffuseTexture;
uniform float absorption;
uniform float scattering;

void main(){
    
    vec3 lightDirection = normalize( lightPosition - pos );
    float spotlightIntensity = dot(-lightDirection, spotLightDirection);
    spotlightIntensity = clamp((spotlightIntensity - 0.8 )/(0.8 - 0.75), 0.0f, 1.0f);
    float attenuation = exp( - (absorption+scattering)*distance);
    diffuseOut = attenuation * spotlightIntensity * vec4(lightColor,1.0f) * texture( diffuseTexture, uv );
    normalOut = vec4( normalize( norm ), 1.0f );
    positionOut = vec4( pos, 1.0f );
}