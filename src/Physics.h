#pragma once

#include "Common.h"
#include "Entity.h"

namespace Physics
{
	Vec2 GetOverlap(Entity a, Entity b);
	Vec2 GetPreviousOverlap(Entity a, Entity b);
	bool IsInside(const Vec2& pos, Entity e);
}