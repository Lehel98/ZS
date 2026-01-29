#pragma once

struct GLFWwindow;

class Input
{
public:
    static void Initialize(GLFWwindow* window);
    static void Update();

    static bool IsKeyPressed(int key);

private:
    static GLFWwindow* windowHandle;
};