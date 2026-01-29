#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include "Entity.h"

bool WouldCollide(
    const Entity& movingEntity,
    const glm::vec3& proposedPosition,
    const std::vector<Entity*>& worldEntities);

void MoveEntityWithCollision(
    Entity& entity,
    const glm::vec3& movement,
    const std::vector<Entity*>& worldEntities);