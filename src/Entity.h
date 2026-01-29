#pragma once

#include "Transform.h"

struct Entity
{
    Transform transform;

    glm::vec3 color;
    bool useVertexColor;

    bool hasCollision;

    Entity() : color(1.0f, 1.0f, 1.0f), useVertexColor(false), hasCollision(false)
    {
    }
};
