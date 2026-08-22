#version 460 core

in vec2 uv;

out vec4 FragColor;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D positionTexture;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 spotLightDirection;
uniform mat4 lightViewMatrix;
uniform mat4 lightProjectionMatrix;
uniform sampler2D lightDepthTexture;
uniform sampler2D raymarchResult;
uniform float absorption;
uniform float scattering;
uniform vec3 cameraPosition;
uniform float lpvStrength;

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

vec3 calculateLPVIllumination( vec3 position, vec3 normal ){

    vec3 scaledPos = (position / lpv.scale) * 0.5f + 0.5f;
    scaledPos.y -= 0.45f;
    float cellSize = 1.0f / lpv.resolution.x;

    vec4 surfaceSH = SHCoefficients(normalize(-normal));

    vec4 shR = texture( lpv.r, scaledPos );
    vec4 shG = texture( lpv.g, scaledPos );
    vec4 shB = texture( lpv.b, scaledPos );

    float r = max(0.0f, dot(shR, surfaceSH));
    float g = max(0.0f, dot(shG, surfaceSH));
    float b = max(0.0f, dot(shB, surfaceSH));

    return vec3(r, g, b);

}

float calculateDiffuseLight( vec3 position, vec3 normal ){

    vec3 lightDirection = normalize( lightPosition - position );
    float diffuse = dot( lightDirection, normal );

    mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;
    vec4 lightSpacePosition = lightProjectionMatrix * lightViewMatrix * vec4( position, 1.0f );
    vec3 shadowMapUV = lightSpacePosition.xyz / lightSpacePosition.w;
    shadowMapUV = shadowMapUV * 0.5f + 0.5f;
    float shadow = 0.0;

    vec3 fragmentDirection = normalize((lightViewMatrix * vec4( position, 1.0f )).xyz);
    float closestDepth = texture( lightDepthTexture, shadowMapUV.xy ).r;
    float currentDepth = shadowMapUV.z;

    float bias = max(0.04 * (1.0 - dot( normal, lightDirection )), 0.01);  

    vec2 texelSize = 1.0 / textureSize(lightDepthTexture, 0);
    for(int x = -2; x <= 2; ++x)
    {
        for(int y = -2; y <= 2; ++y)
        {
            float pcfDepth = texture(lightDepthTexture, shadowMapUV.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }

    shadow /= 25.0;
    float spotlightIntensity = dot(-lightDirection, spotLightDirection);
    spotlightIntensity = clamp((spotlightIntensity - 0.8 )/(0.8 - 0.75), 0.0f, 1.0f);

    float attenuation = exp( -(absorption + scattering)*length(lightPosition - position));

    return max(attenuation * spotlightIntensity * diffuse * (1.0f - shadow), 0.0f);

}

void main(){
    vec3 normal = texture( normalTexture, uv ).rgb;
    vec3 position = texture( positionTexture, uv ).rgb;
    vec3 lpvLight = lpvStrength * calculateLPVIllumination( position, normal );
    vec3 diffuse = lightColor*vec3(calculateDiffuseLight( position, normal ));
    vec4 fog = texture( raymarchResult, uv);
    float cameraAttenuation = exp( -(absorption + scattering)*length(cameraPosition - position)); 
    vec3 col = (1.0f-fog.a)*(cameraAttenuation * (lpvLight + diffuse) * texture( diffuseTexture, uv).rgb) + fog.rgb;
    FragColor = vec4(col,1.0f);
    //FragColor = vec4((lpvLight + diffuse), 1.0f) * texture( diffuseTexture, uv ) + fog;
}