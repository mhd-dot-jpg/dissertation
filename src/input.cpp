#pragma once

#include "input.h"
#include <cstring>

unsigned char previousKeys[GLFW_KEY_LAST + 1];
unsigned char keys[GLFW_KEY_LAST + 1];

double mouseX = 0, mouseY = 0;
double mouseXprev = 0, mouseYprev = 0;
float mouseMoveX = 0, mouseMoveY = 0;

bool mouseButtons[3];

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) keys[key] = 1;
    if (action == GLFW_RELEASE) keys[key] = 0;
}

void calcMouseMovement(GLFWwindow* window) {
    mouseXprev = mouseX;
    mouseYprev = mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {

    mouseMoveX = xpos - mouseX;
    mouseMoveY = ypos - mouseY;

}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){

    if(button < 3){
        if(action == GLFW_PRESS) mouseButtons[button] = true;
        if(action == GLFW_RELEASE) mouseButtons[button] = false;
    }

}

void setCallbacks(GLFWwindow* window) {
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetMouseButtonCallback( window, mouseButtonCallback );
}

unsigned char getKey(int key) {
    return keys[key];
}

unsigned char keyPressed(int key) {
    unsigned char c = keys[key];
    return c & ~previousKeys[key];
}

float getMouseHorizontalMovement() {
    return mouseX - mouseXprev;
}

float getMouseVerticalMovement() {
    return mouseY - mouseYprev;
}

void updateInput() {
    std::memcpy(previousKeys, keys, (GLFW_KEY_LAST + 1) * sizeof(unsigned char));
    mouseMoveX = 0.0;
    mouseMoveY = 0.0;
}

bool mouseButtonDown(int button){
    return mouseButtons[button];
}