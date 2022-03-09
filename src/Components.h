#pragma once

#include "Animation.h"
#include "Assets.h"

class Component
{
public:
	bool has = false;
};

class CTransform : public Component
{
public:
	Vec2 pos		= { 0.0, 0.0 };
	Vec2 prevPos	= { 0.0, 0.0 };
	Vec2 scale		= { 1.0, 1.0 };
	Vec2 velocity	= { 0.0, 0.0 };
	float angle		= 0;

	CTransform() {}
	CTransform(const Vec2& p)
		: pos(p), prevPos(p) {}
	CTransform(const Vec2& p, const Vec2& sp, const Vec2& sc, float a)
		: pos(p), prevPos(p), velocity(sp), scale(sc), angle(a) {}
};

class CLifespan : public Component
{
public:
	int remaining	= 0;	// amount of lifespan remaining on the entity 
	int total		= 0;	// the total initial amount of lifespan
	CLifespan() {}
	CLifespan(int total)
		: remaining(total), total(total) {}
};

class CInput : public Component
{
public:
	bool up			= false;
	bool down		= false;
	bool left		= false;
	bool right		= false;
	bool shoot		= false;
	bool canShoot	= true;
	bool canJump	= false;

	CInput() {}
};

class CBoundingBox : public Component
{
public:
	Vec2 size;
	Vec2 halfSize;
	CBoundingBox() {}
	CBoundingBox(const Vec2& s)
		: size(s), halfSize(s/2) {}
};

class CAnimation : public Component
{
public:
	Animation animation;
	bool repeat = false;
	CAnimation() {}
	CAnimation(const Animation& animation, bool r)
		: animation(animation), repeat(r) {}
};

class CGravity : public Component
{
public:
	float gravity = 0;
	CGravity() {}
	CGravity(float g) : gravity(g) {}
};

class CState : public Component
{
public:
	std::string state		  = "jumping";
	std::string previousState = "jumping";
	CState() {}
	CState(const std::string& s) : state(s) {}
};