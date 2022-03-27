#pragma once

class Vec2
{
public:

	float x = 0;
	float y = 0;

	Vec2();
	Vec2(float xin, float yin);

	Vec2 operator +   (const Vec2& rhs) const;
	Vec2 operator -   (const Vec2& rhs) const;
	Vec2 operator /   (const float val) const;
	Vec2 operator *   (const float val) const;
	Vec2& operator += (const Vec2& rhs);
	Vec2& operator -= (const Vec2& rhs);
	Vec2& operator *= (const float val);
	Vec2& operator /= (const float val);

	bool operator == (const Vec2& rhs) const;
	bool operator != (const Vec2& rhs) const;

	Vec2 abs();
	float mag();
	Vec2 norm();
	Vec2 perp();
	float dot(const Vec2& rhs);
	float cross(const Vec2& rhs);
	float dist(const Vec2& rhs) const;
};