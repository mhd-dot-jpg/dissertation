#version 460 core

layout (points) in;
layout (points, max_vertices = 1) out;

uniform sampler2D positionTexture;
layout (location=0) uniform ivec3 lpvDimensions;
layout (location=1) uniform vec3 lpvScale;

out vec3 vertexPosition;

void main(){

    vec4 wpos = textureLod(positionTexture, gl_in[0].gl_Position.xy, 0.0);
    if(wpos.w > 0.0f){
        vec3 scaledPos = (wpos.xyz / lpvScale);

        vec2 fpos = vec2( scaledPos.x, scaledPos.y - 0.9f );
        int layer = int((scaledPos.z * 0.5f + 0.5f) * lpvDimensions.z);

        gl_Position = vec4(fpos, 0.0, 1.0f);
        gl_Layer = layer;
        vertexPosition = gl_in[0].gl_Position.xyz;
        EmitVertex();
    }

    EndPrimitive();

}