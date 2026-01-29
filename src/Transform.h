#pragma once

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

struct Transform
{
    glm::vec3 position;
    glm::vec3 rotationDegrees;
    glm::vec3 scale;

    Transform();

    glm::mat4 GetModelMatrix() const;
};