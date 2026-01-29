#pragma once

#include "Entity.h"
#include "AABB.h"

AABB ComputeAABB(const Entity& entity);
bool Intersects(const AABB& a, const AABB& b);