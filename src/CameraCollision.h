#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include "Entity.h"

glm::vec3 ComputeCameraPositionWithCollision(
    const Entity& player,
    const glm::vec3& desiredCameraPosition,
    const std::vector<Entity*>& worldEntities);