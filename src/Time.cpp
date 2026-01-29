#include "Time.h"

#include <GLFW/glfw3.h>

float Time::deltaTime = 0.0f;
float Time::lastFrameTime = 0.0f;

void Time::Update()
{
    float currentTime = static_cast<float>(glfwGetTime());
    deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;
}

float Time::GetDeltaTime()
{
    return deltaTime;
}