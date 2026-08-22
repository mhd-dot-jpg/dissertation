#version 460 core

#define PI 3.14159265f

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout (rgba16f, binding = 0) uniform image3D lpv0r;
layout (rgba16f, binding = 1) uniform image3D lpv0g;
layout (rgba16f, binding = 2) uniform image3D lpv0b;

layout (rgba16f, binding = 3) uniform image3D lpv1r;
layout (rgba16f, binding = 4) uniform image3D lpv1g;
layout (rgba16f, binding = 5) uniform image3D lpv1b;

layout (std430, binding = 6) buffer tfBuffer {
    vec4 tfs [26];
};

layout (std430, binding = 7) buffer gammaBuffer {
    float gamma [26];
};

layout (location = 1) uniform ivec3 lpvDimensions;
layout (location = 2) uniform vec3 lpvScale;
layout (location = 3) uniform uvec3 gvDimensions;
layout (location = 4) uniform vec3 gvScale;
layout (location = 5) uniform int step;
layout (location = 6) uniform sampler3D gvSH;
layout (location = 7) uniform float occlusionStrength;
layout (location = 8) uniform sampler3D singleScatteredSH;
layout (location = 9) uniform float absorption;
layout (location = 10) uniform float scattering;

vec4 SHCoefficients(vec3 n){
    return vec4(
        0.282094792,
        -0.488602512 * n.y,
        0.488602512 * n.z,
        -0.488602512 * n.x
    );
}

vec4 cosineSHCoefficients(vec3 n){
    return vec4(
        0.8862269,
        -1.0233267 * n.y,
        1.0233267 * n.z,
        -1.0233267 * n.x
    );
}

void main(){

    ivec3 cellPosition = ivec3(gl_GlobalInvocationID);
    vec3 cellSize = lpvScale / vec3(lpvDimensions);

    vec4 shR = vec4(0.0f);
    vec4 shG = vec4(0.0f);
    vec4 shB = vec4(0.0f);

    int neighbourIndex = 0;
    // for each neighbour
    for (int dx = -1; dx <= 1; dx++)
    {
        for (int dy = -1; dy <= 1; dy++)
        {
            for (int dz = -1; dz <= 1; dz++)
            {
                if (!(dx == 0 && dy == 0 && dz == 0))
                {
                    vec4 transferFunction = tfs[neighbourIndex];
                    vec3 neighbourDirection = vec3(dx, dy, dz);
                    ivec3 neighbourLocation = cellPosition + ivec3(dx, dy, dz);

                    vec4 neighbourSHR = imageLoad(lpv0r, neighbourLocation);
                    vec4 neighbourSHG = imageLoad(lpv0g, neighbourLocation);
                    vec4 neighbourSHB = imageLoad(lpv0b, neighbourLocation);

                    vec3 singleScatteredUV = vec3(neighbourLocation) / vec3(lpvDimensions);
                    vec4 singleScatteredLight = texture( singleScatteredSH, singleScatteredUV );
                    vec3 blockerUV = (vec3(cellPosition) + 0.5f * neighbourDirection) / vec3(lpvDimensions);
                    vec4 blockerSH = texture( gvSH, blockerUV );
                    float occlusion = 0.0f;
                    
                    if(step == 0)
                    {
                        neighbourSHR.r += singleScatteredLight.r;
                        neighbourSHG.r += singleScatteredLight.g;
                        neighbourSHB.r += singleScatteredLight.b;
                    }
                    else
                    {
                        vec4 neighbourDirectionSH = SHCoefficients(-normalize(neighbourDirection));
                        occlusion = max(0.0f, occlusionStrength*dot(transferFunction, blockerSH));
                    }

                    float lambda = length(cellSize) * (absorption + scattering);
                    float lambdaS = length(cellSize) * scattering;
                    vec3 inScatteredRadiance = gamma[neighbourIndex] * lambdaS * 2.0f*sqrt(PI)*vec3( neighbourSHR.r, neighbourSHG.r, neighbourSHB.r );
                    inScatteredRadiance = max(inScatteredRadiance, vec3(0.0f));

                    float iR = (1.0f - occlusion) * ( (1.0f - lambda) * max(0.0f, dot(neighbourSHR, transferFunction)) + inScatteredRadiance.r );
                    float iG = (1.0f - occlusion) * ( (1.0f - lambda) * max(0.0f, dot(neighbourSHG, transferFunction)) + inScatteredRadiance.g );
                    float iB = (1.0f - occlusion) * ( (1.0f - lambda) * max(0.0f, dot(neighbourSHB, transferFunction)) + inScatteredRadiance.b );

                    neighbourIndex++;

                    shR += iR * transferFunction;
                    shG += iG * transferFunction;
                    shB += iB * transferFunction;
                }
            }
        }
    }
    //for(int i = 0; i < 6; i++){

    //    vec4 neighbourSHR = imageLoad(lpv0SHR, cellPosition - directions[i]);
    //    vec4 neighbourSHG = imageLoad(lpv0SHG, cellPosition - directions[i]);
    //    vec4 neighbourSHB = imageLoad(lpv0SHB, cellPosition - directions[i]);

    //    // for each face
    //    for(int j = 0; j < 6; j++){
    //        // Skip the face pointing in the same direction as the neighbour
    //        if(dot(directions[i], directions[j]) >= 0.0f){
    //            // Calculate SH projection of face direction
    //            vec3 faceDirection = normalize(0.5f*directions[j] + directions[j]); 
    //            vec4 faceDirectionSH = SHCoefficients(faceDirection);

    //            // Evaluate intensity (dot product of SH coefficients and direction of face)
    //            float iR = max(0.0f, dot(neighbourSHR, faceDirectionSH));
    //            float iG = max(0.0f, dot(neighbourSHG, faceDirectionSH));
    //            float iB = max(0.0f, dot(neighbourSHB, faceDirectionSH));

    //            float occlusion = 0.0f;

    //            // Only calculate blocking after the first step
    //            if(propagationStep > 0){
    //                
    //                vec3 gvCell = vec3(cellPosition - directions[i]) + 0.5f * faceDirection + 0.5f + 0.5f*cellSize;
    //                gvCell = gvCell / vec3(gvDimensions); 

    //                occlusion = clamp(occlusionStrength*dot(SHCoefficients(-faceDirection), texture(gvSH, gvCell)), 0.0f, 1.0f);

    //            }
    //            
    //            vec3 flux = (1.0f - occlusion) * vec3(iR, iG, iB);

    //            // Scale intensity by subtended solid angle to get propagated flux
    //            flux *= mix(0.423431354f / PI, 0.4006696844f / PI, dot(directions[i], directions[j]));

    //            // Reproject flux as coming from point light in the centre of the cell
    //            vec3 faceNormal = directions[j];
    //            vec4 faceReprojectionSH = cosineSHCoefficients(faceNormal);

    //            shR += flux.r * faceReprojectionSH;
    //            shG += flux.g * faceReprojectionSH;
    //            shB += flux.b * faceReprojectionSH;
    //        }

    //    }

    //}

    imageStore(lpv1r, cellPosition, shR);
    imageStore(lpv1g, cellPosition, shG);
    imageStore(lpv1b, cellPosition, shB);

}
