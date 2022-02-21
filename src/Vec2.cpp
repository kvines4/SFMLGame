#include "Vec2.h"
#include <cmath>

Vec2::Vec2()							  {}
Vec2::Vec2(float x, float y) : x(x), y(y) {}

Vec2 Vec2::operator +   (const Vec2& rhs) const { return Vec2(this->x + rhs.x, this->y + rhs.y);    }
Vec2 Vec2::operator -   (const Vec2& rhs) const { return Vec2(this->x - rhs.x, this->y - rhs.y);    }
Vec2 Vec2::operator /   (const float val) const { return Vec2(this->x / val,   this->y / val);      }
Vec2 Vec2::operator *   (const float val) const { return Vec2(this->x * val,   this->y * val);      }

Vec2& Vec2::operator += (const Vec2& rhs)       { this->x += rhs.x; this->y += rhs.y; return *this; }
Vec2& Vec2::operator -= (const Vec2& rhs)       { this->x -= rhs.x; this->y -= rhs.y; return *this; }
Vec2& Vec2::operator *= (const float val)       { this->x *= val;   this->y *= val;   return *this; }
Vec2& Vec2::operator /= (const float val)       { this->x /= val;   this->y /= val;   return *this; }

bool Vec2::operator ==  (const Vec2& rhs) const { return (this->x == rhs.x) && (this->y == rhs.y);  }
bool Vec2::operator !=  (const Vec2& rhs) const { return (this->x != rhs.x) && (this->y != rhs.y);  }

float Vec2::dist(const Vec2& rhs) const { 
    float dy = this->y - rhs.y;
    float dx = this->x - rhs.x;
    return sqrt((dy * dy) + (dx * dx));
}
