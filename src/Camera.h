#pragma once

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

struct GLFWwindow;

class Camera
{
public:
    explicit Camera(GLFWwindow* window);

    void Update();

    glm::vec3 GetPosition() const;
    glm::vec3 GetForwardDirection() const;
    glm::vec3 GetRightDirection() const;
    void SetPosition(glm::vec3 newPosition);

    glm::mat4 GetViewMatrix() const;

private:
    void UpdateMouseLook();
    void UpdateMovement();

private:
    GLFWwindow* windowHandle;

    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 right;
    glm::vec3 up;

    float yaw;
    float pitch;

    float movementSpeed;
    float mouseSensitivity;

    float fixedHeight;

    double lastMouseX;
    double lastMouseY;
    bool firstMouse;
};