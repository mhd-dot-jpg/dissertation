#pragma once

#include <GLFW/glfw3.h>

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void setCallbacks(GLFWwindow* window);

unsigned char getKey(int key);
unsigned char keyPressed(int key);

float getMouseHorizontalMovement();
float getMouseVerticalMovement();
void updateInput();
bool mouseButtonDown( int button );
void calcMouseMovement(GLFWwindow* window);
