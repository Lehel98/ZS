#include "CameraCollision.h"
#include "CollisionSystem.h"
#include "AABB.h"
#include "Entity.h"
#include <glm/glm.hpp>
#include <glm/geometric.hpp>

glm::vec3 ResolveCameraCollision(
    const Entity& player,
    const glm::vec3& desiredCameraPosition,
    float cameraRadius,
    const std::vector<Entity*>& worldEntities)
{
    // ---- Player capsule world space ----
    const float playerRadius = player.collision.capsule.radius;
    const float playerHeight = player.collision.capsule.height;

    glm::vec3 playerCenter =
        player.transform.position +
        glm::vec3(0.0f, playerRadius + (playerHeight - 2.0f * playerRadius) * 0.5f, 0.0f);

    glm::vec3 capsuleBase = playerCenter + player.collision.localOffset + glm::vec3(0.0f, playerRadius, 0.0f);
    glm::vec3 capsuleTip = capsuleBase + glm::vec3(0.0f, playerHeight - 2.0f * playerRadius, 0.0f);

    constexpr int Steps = 24;

    const float minDistance = playerRadius + cameraRadius + 0.05f;

    // ============================================================
    // 1) Horizontal minimum distance clamp
    // ============================================================

    glm::vec3 adjustedDesired = desiredCameraPosition;

    glm::vec3 horizontal =
    {
        adjustedDesired.x - playerCenter.x,
        0.0f,
        adjustedDesired.z - playerCenter.z
    };

    float horizontalDist = glm::length(horizontal);

    if (horizontalDist < minDistance)
    {
        if (horizontalDist < 0.0001f)
        {
            // Kamera pontosan a player felett – válasszunk egy irányt
            horizontal = glm::vec3(0.0f, 0.0f, -1.0f);
        }
        else
        {
            horizontal = glm::normalize(horizontal);
        }

        adjustedDesired.x =
            playerCenter.x + horizontal.x * minDistance;

        adjustedDesired.z =
            playerCenter.z + horizontal.z * minDistance;
    }

    // ============================================================
    // 2) Sweep from player to desired position
    // ============================================================

    glm::vec3 direction = adjustedDesired - playerCenter;

    float length = glm::length(direction);

    if (length < 0.0001f)
        return adjustedDesired;

    direction /= length;

    glm::vec3 lastValidPosition = playerCenter + direction * minDistance;

    for (int i = 1; i <= Steps; ++i)
    {
        float t = static_cast<float>(i) / Steps;

        glm::vec3 testPosition =
            playerCenter + direction * (length * t);

        bool collided = false;

        // World AABB collision
        for (const Entity* entity : worldEntities)
        {
            if (entity == &player)
                continue;

            if (entity->collision.type != CollisionShape::Type::AABB)
                continue;

            AABB box = ComputeWorldAABB(*entity);

            if (IntersectSphereVsAABB(testPosition, cameraRadius, box))
            {
                collided = true;
                break;
            }
        }

        // Player capsule collision
        if (!collided)
        {
            if (IntersectSphereVsCapsule(testPosition, cameraRadius, capsuleBase, capsuleTip, playerRadius))
            {
                collided = true;
            }
        }

        if (collided)
            return lastValidPosition;

        lastValidPosition = testPosition;
    }

    return adjustedDesired;
}