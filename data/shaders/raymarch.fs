#version 460 core

#define MAX_STEPS 64
#define STEP_SIZE  0.1f

in vec2 uv;

out vec4 FragColor;

uniform vec3 cameraPosition;
uniform sampler2D positionTexture;
uniform float absorption;
uniform float scattering;

uniform struct LPV{
    sampler3D r;
    sampler3D g;
    sampler3D b;
    ivec3 resolution;
    vec3 scale;
} lpv;

vec4 SHCoefficients(vec3 n){
    return vec4(
        0.282094792,
        -0.488602512 * n.y,
        0.488602512 * n.z,
        -0.488602512 * n.x
    );
}

vec3 calculateLPVIllumination( vec3 position, vec4 directionSH ){

    vec3 scaledPos = (position / lpv.scale) * 0.5f + 0.5f;
    scaledPos.y -= 0.45f;

    vec4 shR = texture( lpv.r, scaledPos );
    vec4 shG = texture( lpv.g, scaledPos );
    vec4 shB = texture( lpv.b, scaledPos );

    float r = max(0.0f, dot(shR, directionSH));
    float g = max(0.0f, dot(shG, directionSH));
    float b = max(0.0f, dot(shB, directionSH));

    return vec3(shR.r, shG.r, shB.r);max(vec3(r, g, b), vec3(0.0f));

}

void main(){

    vec3 rayDirection = texture( positionTexture, uv ).xyz - cameraPosition;
    float maxDistance = length( rayDirection );
    rayDirection = normalize( rayDirection );
    vec4 directionSH = SHCoefficients( -rayDirection );

    vec4 totalIllumination = vec4(0.0f);
    float attenuationStep = exp( -(absorption + scattering) * STEP_SIZE );
    float attenuation = attenuationStep;

    for( int i = 0; i < MAX_STEPS && i * STEP_SIZE < maxDistance; i++){
        vec3 samplePosition = cameraPosition + i*STEP_SIZE*rayDirection;
        vec3 illumination = calculateLPVIllumination( samplePosition, directionSH );
        float intensity = (illumination.r + illumination.g + illumination.b) / 3.0f;
        totalIllumination += scattering*attenuation * STEP_SIZE * vec4( illumination, intensity );
        attenuation *= attenuationStep;
    }

    FragColor = totalIllumination;

}
