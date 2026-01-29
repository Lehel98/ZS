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
    movementSpeed(5.0f),
    mouseSensitivity(0.1f),
    lastMouseX(0.0),
    lastMouseY(0.0),
    firstMouse(true)
{
    fixedHeight = position.y;
    glfwSetInputMode(windowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Camera::Update()
{
    UpdateMouseLook();
    //UpdateMovement();
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

    pitch = std::clamp(pitch, -89.0f, 89.0f);

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    forward = glm::normalize(direction);
    right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    up = glm::normalize(glm::cross(right, forward));
}

void Camera::UpdateMovement()
{
    float velocity = movementSpeed * Time::GetDeltaTime();

    glm::vec3 flatForward =
    {
        forward.x,
        0.0f,
        forward.z
    };

    flatForward = glm::normalize(flatForward);

    glm::vec3 flatRight =
    {
        right.x,
        0.0f,
        right.z
    };

    flatRight = glm::normalize(flatRight);

    glm::vec3 movementDirection(0.0f);

    if (Input::IsKeyPressed(GLFW_KEY_W))
    {
        movementDirection += flatForward;
    }

    if (Input::IsKeyPressed(GLFW_KEY_S))
    {
        movementDirection -= flatForward;
    }

    if (Input::IsKeyPressed(GLFW_KEY_D))
    {
        movementDirection += flatRight;
    }

    if (Input::IsKeyPressed(GLFW_KEY_A))
    {
        movementDirection -= flatRight;
    }

    if (glm::length(movementDirection) > 0.0f)
    {
        movementDirection = glm::normalize(movementDirection);

        position += movementDirection * velocity;
    }

    position.y = fixedHeight;
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