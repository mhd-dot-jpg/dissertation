#version 460 core

layout (points) in;
layout (points, max_vertices = 8) out;

uniform sampler2D positionTexture;
uniform vec3 lightPosition;
uniform vec3 spotLightDirection;
uniform float absorption;
uniform float scattering;
layout (location=0) uniform ivec3 lpvDimensions;
layout (location=1) uniform vec3 lpvScale;

out vec3 vertexPosition;
out float intensity;

float rand(float n){return 1.0f - 2.0f*fract(sin(n) * 43758.5453123);}

void main(){

    vec4 wpos = textureLod(positionTexture, gl_in[0].gl_Position.xy, 0.0);
    vec3 lightDirection = normalize( lightPosition - wpos.xyz );

    if(wpos.w > 0.0f && length(wpos) > 0.0f && dot(-lightDirection, spotLightDirection) >= 0.8f){
        
        vec3 step = ( wpos.xyz - lightPosition ) / 8.0f;
        float attenuation = exp( - (absorption+scattering)*length(step));
        float dA = attenuation;
        for(int i = 0; i < 8; i++){ 

            vec3 samplePosition = lightPosition + i*step;
            samplePosition += 0.1f*(rand(samplePosition.x) + rand(samplePosition.y) + rand(samplePosition.z))*step;

            vec3 scaledPos = samplePosition / lpvScale; 
            vec2 fpos = vec2( scaledPos.x, scaledPos.y - 0.9f );
            int layer = int((scaledPos.z * 0.5f + 0.5f) * lpvDimensions.z);

            gl_Position = vec4(fpos, 0.0, 1.0f);
            gl_Layer = layer;
            vertexPosition = gl_in[0].gl_Position.xyz;
            intensity = attenuation;
            attenuation *= dA;
            EmitVertex();

        }

    }

    EndPrimitive();

}