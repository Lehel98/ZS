#include "Collision.h"

AABB ComputeAABB(const Entity& entity)
{
    const glm::vec3 halfSize = entity.transform.scale * 0.5f;

    AABB box;
    box.min = entity.transform.position - halfSize;
    box.max = entity.transform.position + halfSize;

    return box;
}

bool Intersects(const AABB& a, const AABB& b)
{
    if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
    if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
    if (a.max.z < b.min.z || a.min.z > b.max.z) return false;

    return true;
}