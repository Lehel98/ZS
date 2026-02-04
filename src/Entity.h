#pragma once

#include "Transform.h"
#include "CollisionShape.h"

struct Entity
{
    Transform transform;

    // Collision
    CollisionShape collision;

    // Render
    glm::vec3 color = glm::vec3(1.0f);
    bool useVertexColor = false;
};