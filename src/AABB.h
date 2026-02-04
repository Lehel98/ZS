#pragma once

#include <glm/vec3.hpp>

struct Entity; // elõre deklaráljuk, hogy ne legyen körkörös include

struct AABB
{
    glm::vec3 min;
    glm::vec3 max;
};

AABB ComputeWorldAABB(const Entity& entity);