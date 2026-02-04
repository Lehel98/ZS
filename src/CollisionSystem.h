#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include "AABB.h"

bool IntersectSphereVsAABB(const glm::vec3& sphereCenter, float sphereRadius, const AABB& box);

bool IntersectCapsuleVsAABB(const glm::vec3& capsuleBase, const glm::vec3& capsuleTip, float capsuleRadius, const AABB& box);