#include "Physics.h"
#include "Components.h"

//float Physics::DEGTORAD = 0.017453f;

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	// both entities require a bounding box to collide
	if (!a->hasComponent<CBoundingBox>() || !b->hasComponent<CBoundingBox>()) { return Vec2(0, 0); }

	// get the (x,y) distances between the two bounding boxes
	Vec2 delta = (a->getComponent<CTransform>().pos - b->getComponent<CTransform>().pos).abs();

	// calculate the vertical and horizontal overlaps
	Vec2 boxSize = (a->getComponent<CBoundingBox>().halfSize + b->getComponent<CBoundingBox>().halfSize);

	// return if overlapping in both x&y axis, otherwise return 0
	return Vec2(boxSize.x - delta.x, boxSize.y - delta.y);
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	// both entities require a bounding box to collide
	if (!a->hasComponent<CBoundingBox>() || !b->hasComponent<CBoundingBox>()) { return Vec2(0, 0); }

	// get the (x,y) distances between the two bounding boxes
	Vec2 delta = (a->getComponent<CTransform>().prevPos - b->getComponent<CTransform>().prevPos).abs();

	// calculate the vertical and horizontal overlaps
	Vec2 boxSize = (a->getComponent<CBoundingBox>().halfSize + b->getComponent<CBoundingBox>().halfSize);

	// return if overlapping in both x&y axis, otherwise return 0
	return Vec2(boxSize.x - delta.x, boxSize.y - delta.y);
}

bool Physics::IsInside(const Vec2& pos, std::shared_ptr<Entity> e)
{
	// if the entity doesn't have an animation, we can't be 'inside' it
	if (!e->hasComponent<CAnimation>()) { return false; }

	auto halfSize = e->getComponent<CAnimation>().animation.getSize() / 2;
	
	// determine the delta vector (distance) between both entities
	Vec2 delta = (e->getComponent<CTransform>().pos - pos).abs();

	// uf the click is within the x and y bounds of the halfsize, we're inside
	return (delta.x <= halfSize.x) && (delta.y <= halfSize.y);
}
