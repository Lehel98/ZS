#pragma once

#include <vector>
#include <glm/vec3.hpp>

struct Entity;

glm::vec3 ResolveCameraCollision(
    const glm::vec3& pivot,
    const glm::vec3& desiredPosition,
    float cameraRadius,
    const std::vector<Entity*>& worldEntities);