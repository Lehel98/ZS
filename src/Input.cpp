#include "Input.h"

#include <GLFW/glfw3.h>

GLFWwindow* Input::windowHandle = nullptr;

void Input::Initialize(GLFWwindow* window)
{
    windowHandle = window;
}

void Input::Update()
{
    // késõbb ide jön edge detection (pressed / released)
}

bool Input::IsKeyPressed(int key)
{
    return glfwGetKey(windowHandle, key) == GLFW_PRESS;
}