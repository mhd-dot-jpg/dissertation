#pragma once

#include <map>
#include <string>
#include <glad/glad.h>
#include "glm/glm.hpp"

#include "framebuffer.h"
#include "shader.h"
#include "camera.h"
#include "model.h"
#include "light.h"

void initRenderer();
void render( std::vector<Model>& objects, Camera& camera, Shader& shader, FrameBuffer& framebuffer );
void renderGBuffer( FrameBuffer& screenBuffer, FrameBuffer& gBuffer, Camera& cam, SpotLight& light, FrameBuffer& shadowMap,
                    FrameBuffer& lpv, glm::ivec3 lpvResolution, glm::vec3 lpvSize, FrameBuffer& raymarchBuffer,
                    float absorption, float scattering, float lpvStrength );
void raymarchLPV( FrameBuffer& buffer, FrameBuffer& gBuffer, Camera& camera, FrameBuffer& lpv, glm::ivec3 lpvResolution, glm::vec3 lpvSize, float absorption, float scattering );
void renderShadowMap( std::vector<Model>& objects, SpotLight light, Shader& shader, FrameBuffer& framebuffer, float absorption, float scattering );
