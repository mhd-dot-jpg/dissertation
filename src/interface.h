#pragma once

#include "glm/glm.hpp"

#include "light.h"

void initUI( GLFWwindow* window );
void startUIFrame();
void renderUI( SpotLight& light, float& occlusionStrength, float& absorption, float& scattering, float& lpvStrength );
