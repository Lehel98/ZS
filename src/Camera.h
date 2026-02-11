#pragma once

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

struct GLFWwindow;

class Camera
{
public:
    explicit Camera(GLFWwindow* window);

    void Update();
    //void UpdateThirdPerson(const glm::vec3& pivot, float cameraDistance, float cameraHeight, float minDegree, float maxDegree);
    glm::vec3 ComputeDesiredPosition(
        const glm::vec3& pivot,
        float cameraDistance,
        float cameraHeight,
        float minDegree,
        float maxDegree);


    glm::vec3 GetPosition() const;
    glm::vec3 GetForwardDirection() const;
    glm::vec3 GetRightDirection() const;
    void SetPosition(glm::vec3 newPosition);

    glm::mat4 GetViewMatrix() const;

private:
    void UpdateMouseLook();
    void UpdateVectorsFromAngles();

private:
    GLFWwindow* windowHandle;

    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 right;
    glm::vec3 up;

    float yaw = 180.0f;
    float pitch = 90.0f;

    float MinPitch = -90.0f;
    float MaxPitch = 90.0f;

    float mouseSensitivity = 0.1f;

    double lastMouseX;
    double lastMouseY;
    bool firstMouse;
};