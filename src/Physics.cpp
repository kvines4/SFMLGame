#include "Physics.h"
#include "Components.h"

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	auto& boxA		 = a->getComponent<CBoundingBox>();
	auto& transformA = a->getComponent<CTransform>();
	auto& boxB		 = b->getComponent<CBoundingBox>();
	auto& transformB = b->getComponent<CTransform>();
	// calculate the vertical and horizontal differences
	Vec2 delta(abs(transformB.pos.x - transformA.pos.x),
			   abs(transformB.pos.y - transformA.pos.y));
	// calculate the vertical and horizontal overlaps
	Vec2 overlap((boxA.halfSize.x + boxB.halfSize.x - delta.x),
				 (boxA.halfSize.y + boxB.halfSize.y - delta.y));
	// return if overlapping in both x&y axis, otherwise return 0
	return overlap;
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	auto& boxA		 = a->getComponent<CBoundingBox>();
	auto& transformA = a->getComponent<CTransform>();
	auto& boxB		 = b->getComponent<CBoundingBox>();
	auto& transformB = b->getComponent<CTransform>();
	// calculate the vertical and horizontal differences
	Vec2 delta(abs(transformB.prevPos.x - transformA.prevPos.x),
			   abs(transformB.prevPos.y - transformA.prevPos.y));
	// calculate the vertical and horizontal overlaps
	Vec2 overlap((boxA.halfSize.x + boxB.halfSize.x - delta.x),
				 (boxA.halfSize.y + boxB.halfSize.y - delta.y));
	// return if overlapping in both x&y axis, otherwise return 0
	return overlap;
}
