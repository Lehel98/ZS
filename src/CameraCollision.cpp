#include "CameraCollision.h"
#include "CollisionSystem.h"
#include "AABB.h"
#include "Entity.h"

glm::vec3 ResolveCameraCollision(
    const glm::vec3& playerPosition,
    const glm::vec3& desiredCameraPosition,
    float cameraRadius,
    const std::vector<Entity*>& worldEntities)
{
    constexpr int Steps = 24;

    glm::vec3 direction = desiredCameraPosition - playerPosition;
    glm::vec3 lastValidPosition = playerPosition;

    for (int i = 1; i <= Steps; ++i)
    {
        float t = static_cast<float>(i) / Steps;
        glm::vec3 testPosition =
            playerPosition + direction * t;

        bool collided = false;

        for (const Entity* entity : worldEntities)
        {
            if (entity->collision.type != CollisionShape::Type::AABB)
                continue;

            AABB box = ComputeWorldAABB(*entity);

            if (IntersectSphereVsAABB(
                testPosition,
                cameraRadius,
                box))
            {
                collided = true;
                break;
            }
        }

        if (collided)
            return lastValidPosition;

        lastValidPosition = testPosition;
    }

    return desiredCameraPosition;
}