#pragma once

#include "glm/glm.hpp"

#include "framebuffer.h"
#include "camera.h"
#include "light.h"

void initLPV( glm::ivec3 resolution, glm::vec3 size, glm::ivec2 injectionResolution, int propagationSteps );
void injectBlockers( Camera& camera, FrameBuffer& gBuffer, SpotLight& light, FrameBuffer& rsmBuffer );
void injectReflectedLight( FrameBuffer lpvBuffer, FrameBuffer rsmBuffer, SpotLight light, float absorption, float scattering );
void propagateLight( FrameBuffer lpvBuffer, float occlusionStrength, float absorption, float scattering );
