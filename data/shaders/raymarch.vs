#version 460 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTexCoord;

out vec2 uv;

void main(){
    gl_Position = vec4(vPos, 1.0f);
    uv = vTexCoord;
}
