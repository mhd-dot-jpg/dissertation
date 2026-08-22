#version 460 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout (rgba16f, binding = 0) uniform image3D lpvrSrc;
layout (rgba16f, binding = 1) uniform image3D lpvgSrc;
layout (rgba16f, binding = 2) uniform image3D lpvbSrc;

layout (rgba16f, binding = 3) uniform image3D lpvrDest;
layout (rgba16f, binding = 4) uniform image3D lpvgDest;
layout (rgba16f, binding = 5) uniform image3D lpvbDest;

void main(){

    ivec3 texelCoord = ivec3(gl_GlobalInvocationID); 

    vec4 shR = imageLoad(lpvrSrc, texelCoord);
    vec4 shG = imageLoad(lpvgSrc, texelCoord);
    vec4 shB = imageLoad(lpvbSrc, texelCoord);

    vec4 accShR = imageLoad(lpvrDest, texelCoord);
    vec4 accShG = imageLoad(lpvgDest, texelCoord);
    vec4 accShB = imageLoad(lpvbDest, texelCoord);

    imageStore(lpvrDest, texelCoord, accShR + shR);
    imageStore(lpvgDest, texelCoord, accShG + shG);
    imageStore(lpvbDest, texelCoord, accShB + shB);

}
