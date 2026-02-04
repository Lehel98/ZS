#pragma once

#include <vector>
#include <glm/vec3.hpp>

struct Entity;

glm::vec3 ResolveCameraCollision(
    const glm::vec3& playerPosition,
    const glm::vec3& desiredCameraPosition,
    float cameraRadius,
    const std::vector<Entity*>& worldEntities);