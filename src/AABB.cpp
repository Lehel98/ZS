#include "AABB.h"
#include "Entity.h"

AABB ComputeWorldAABB(const Entity& entity)
{
    const CollisionShape& c = entity.collision;

    glm::vec3 center = entity.transform.position + c.localOffset;

    return
    {
        center - c.halfExtents,
        center + c.halfExtents
    };
}