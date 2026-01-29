#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>

Transform::Transform()
    : position(0.0f, 0.0f, 0.0f),
    rotationDegrees(0.0f, 0.0f, 0.0f),
    scale(1.0f, 1.0f, 1.0f)
{
}

glm::mat4 Transform::GetModelMatrix() const
{
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, position);

    model = glm::rotate(
        model,
        glm::radians(rotationDegrees.x),
        glm::vec3(1.0f, 0.0f, 0.0f));

    model = glm::rotate(
        model,
        glm::radians(rotationDegrees.y),
        glm::vec3(0.0f, 1.0f, 0.0f));

    model = glm::rotate(
        model,
        glm::radians(rotationDegrees.z),
        glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, scale);

    return model;
}