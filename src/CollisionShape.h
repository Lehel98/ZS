#pragma once

#include <glm/vec3.hpp>

struct CapsuleShape
{
    float radius = 0.0f;
    float height = 0.0f;
};

struct CollisionShape
{
    enum class Type
    {
        None,
        AABB,
        Sphere,
        Capsule
    };

    Type type = Type::None;

    // közös
    glm::vec3 localOffset = glm::vec3(0.0f);

    // AABB
    glm::vec3 halfExtents = glm::vec3(0.0f);

    // Sphere / Capsule
    float radius = 0.0f;

    // Capsule
    CapsuleShape capsule;
};