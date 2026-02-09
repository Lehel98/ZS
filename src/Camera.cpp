#include "Camera.h"

#include "Input.h"
#include "Time.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

Camera::Camera(GLFWwindow* window)
    : windowHandle(window),
    position(0.0f, 1.5f, 5.0f),
    forward(0.0f, 0.0f, -1.0f),
    right(1.0f, 0.0f, 0.0f),
    up(0.0f, 1.0f, 0.0f),
    yaw(-90.0f),
    pitch(0.0f),
    mouseSensitivity(0.1f),
    lastMouseX(0.0),
    lastMouseY(0.0),
    firstMouse(true)
{
    glfwSetInputMode(windowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    UpdateVectorsFromAngles();
}

void Camera::Update()
{
    UpdateMouseLook();
}

void Camera::UpdateMouseLook()
{
    double mouseX;
    double mouseY;
    glfwGetCursorPos(windowHandle, &mouseX, &mouseY);

    if (firstMouse)
    {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        firstMouse = false;
    }

    float xOffset = static_cast<float>(mouseX - lastMouseX);
    float yOffset = static_cast<float>(lastMouseY - mouseY);

    lastMouseX = mouseX;
    lastMouseY = mouseY;

    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw += xOffset;
    pitch += yOffset;

    pitch = std::clamp(pitch, -90.0f, 90.0f);
}

void Camera::UpdateVectorsFromAngles()
{
    float yawRad = glm::radians(yaw);
    float pitchRad = glm::radians(pitch);

    glm::vec3 dir;
    dir.x = cos(pitchRad) * cos(yawRad);
    dir.y = sin(pitchRad);
    dir.z = cos(pitchRad) * sin(yawRad);

    forward = glm::normalize(dir);
    right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
    up = glm::normalize(glm::cross(right, forward));
}

void Camera::UpdateThirdPerson(
    const glm::vec3& pivot,
    float cameraDistance,
    float cameraHeight,
    float minDegree,
    float maxDegree)
{
    float effectiveMin = std::clamp(minDegree, -90.0f, -45.0f);
    float effectiveMax = std::clamp(maxDegree, 45.0f, 90.0f);

    pitch = std::clamp(pitch, effectiveMin, effectiveMax);

    // Frissítjük az irányvektorokat
    UpdateVectorsFromAngles();

    // Félkör pozíció
    glm::vec3 offset = -forward * cameraDistance;
    offset.y += cameraHeight;

    position = pivot + offset;
}

glm::vec3 Camera::GetPosition() const
{
    return position;
}

glm::vec3 Camera::GetForwardDirection() const
{
    return forward;
}

glm::vec3 Camera::GetRightDirection() const
{
    return right;
}

void Camera::SetPosition(glm::vec3 newPosition)
{
    position = newPosition;
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(position, position + forward, up);
}