#include "CollisionSystem.h"
#include <glm/common.hpp>
#include <glm/geometric.hpp>

bool IntersectSphereVsAABB(const glm::vec3& sphereCenter, float sphereRadius, const AABB& box)
{
    glm::vec3 closestPoint =
    {
        glm::clamp(sphereCenter.x, box.min.x, box.max.x),
        glm::clamp(sphereCenter.y, box.min.y, box.max.y),
        glm::clamp(sphereCenter.z, box.min.z, box.max.z)
    };

    glm::vec3 delta = sphereCenter - closestPoint;

    return glm::dot(delta, delta) <= sphereRadius * sphereRadius;
}

static float Clamp(float v, float minV, float maxV)
{
    return std::max(minV, std::min(v, maxV));
}

static glm::vec3 ClosestPointOnSegment(const glm::vec3& a, const glm::vec3& b, const glm::vec3& p)
{
    glm::vec3 ab = b - a;
    float t = glm::dot(p - a, ab) / glm::dot(ab, ab);
    t = Clamp(t, 0.0f, 1.0f);
    return a + ab * t;
}

bool IntersectCapsuleVsAABB(const glm::vec3& capsuleBase, const glm::vec3& capsuleTip, float capsuleRadius, const AABB& box)
{
    // AABB középpontjához legközelebbi pont a capsule tengelyen
    glm::vec3 boxCenter = (box.min + box.max) * 0.5f;
    glm::vec3 closestPoint =
        ClosestPointOnSegment(capsuleBase, capsuleTip, boxCenter);

    // AABB-hez legközelebbi pont
    glm::vec3 clamped =
    {
        Clamp(closestPoint.x, box.min.x, box.max.x),
        Clamp(closestPoint.y, box.min.y, box.max.y),
        Clamp(closestPoint.z, box.min.z, box.max.z)
    };

    glm::vec3 delta = closestPoint - clamped;
    return glm::dot(delta, delta) <= capsuleRadius * capsuleRadius;
}

bool IntersectSphereVsCapsule(
    const glm::vec3& sphereCenter,
    float sphereRadius,
    const glm::vec3& capsuleBase,
    const glm::vec3& capsuleTip,
    float capsuleRadius)
{
    glm::vec3 closest = ClosestPointOnSegment(capsuleBase, capsuleTip, sphereCenter);

    glm::vec3 delta = sphereCenter - closest;

    float combinedRadius = sphereRadius + capsuleRadius;

    return glm::dot(delta, delta) <= combinedRadius * combinedRadius;
}