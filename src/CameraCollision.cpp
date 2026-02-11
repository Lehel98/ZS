#include "CameraCollision.h"
#include "CollisionSystem.h"
#include "AABB.h"
#include "Entity.h"
#include <glm/glm.hpp>
#include <glm/geometric.hpp>

glm::vec3 ResolveCameraCollision(
    const glm::vec3& pivot,
    const glm::vec3& desiredPosition,
    float cameraRadius,
    const std::vector<Entity*>& worldEntities)
{
    constexpr int Steps = 32;

    glm::vec3 direction = desiredPosition - pivot;
    float length = glm::length(direction);

    if (length < 0.0001f)
        return desiredPosition;

    direction /= length;

    glm::vec3 lastValid = pivot;

    for (int i = 1; i <= Steps; ++i)
    {
        float t = (float)i / Steps;
        glm::vec3 testPos = pivot + direction * (length * t);

        bool collided = false;

        for (const Entity* e : worldEntities)
        {
            if (e->collision.type != CollisionShape::Type::AABB)
                continue;

            AABB box = ComputeWorldAABB(*e);

            if (IntersectSphereVsAABB(testPos, cameraRadius, box))
            {
                collided = true;
                break;
            }
        }

        if (collided)
            return lastValid;

        lastValid = testPos;
    }

    return desiredPosition;
}