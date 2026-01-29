#include "CameraCollision.h"
#include "Collision.h"

glm::vec3 ComputeCameraPositionWithCollision(
    const Entity& player,
    const glm::vec3& desiredCameraPosition,
    const std::vector<Entity*>& worldEntities)
{
    constexpr int Steps = 20;
    constexpr float MinCameraDistance = 1.5f;

    const glm::vec3 playerPosition = player.transform.position;

    glm::vec3 direction = desiredCameraPosition - playerPosition;
    glm::vec3 lastValidPosition = playerPosition;

    for (int i = 1; i <= Steps; ++i)
    {
        float t = static_cast<float>(i) / Steps;
        glm::vec3 testPosition = playerPosition + direction * t;

        if (glm::length(testPosition - playerPosition) < MinCameraDistance)
        {
            return player.transform.position + glm::normalize(direction) * MinCameraDistance;
        }

        Entity cameraProxy;
        cameraProxy.transform.position = testPosition;
        cameraProxy.transform.scale = glm::vec3(0.2f);
        cameraProxy.hasCollision = true;

        AABB cameraBox = ComputeAABB(cameraProxy);

        bool collided = false;

        for (const Entity* entity : worldEntities)
        {
            if (!entity->hasCollision)
                continue;

            if (entity == &player)
                continue;

            AABB otherBox = ComputeAABB(*entity);

            if (Intersects(cameraBox, otherBox))
            {
                collided = true;
                break;
            }
        }

        if (collided)
        {
            return lastValidPosition;
        }

        lastValidPosition = testPosition;
    }

    return desiredCameraPosition;
}