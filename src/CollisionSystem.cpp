#include "CollisionSystem.h"
#include "Collision.h"

bool WouldCollide(
    const Entity& movingEntity,
    const glm::vec3& proposedPosition,
    const std::vector<Entity*>& worldEntities)
{
    Entity testEntity = movingEntity;
    testEntity.transform.position = proposedPosition;

    AABB movingBox = ComputeAABB(testEntity);

    for (const Entity* other : worldEntities)
    {
        if (!other->hasCollision)
            continue;

        if (other == &movingEntity)
            continue;

        AABB otherBox = ComputeAABB(*other);

        if (Intersects(movingBox, otherBox))
        {
            return true;
        }
    }

    return false;
}

void MoveEntityWithCollision(Entity& entity, const glm::vec3& movement, const std::vector<Entity*>& worldEntities)
{
    glm::vec3 newPosition = entity.transform.position;

    // X tengely
    if (movement.x != 0.0f)
    {
        glm::vec3 testPosition = newPosition;
        testPosition.x += movement.x;

        if (!WouldCollide(entity, testPosition, worldEntities))
        {
            newPosition.x = testPosition.x;
        }
    }

    // Z tengely
    if (movement.z != 0.0f)
    {
        glm::vec3 testPosition = newPosition;
        testPosition.z += movement.z;

        if (!WouldCollide(entity, testPosition, worldEntities))
        {
            newPosition.z = testPosition.z;
        }
    }

    entity.transform.position = newPosition;
}