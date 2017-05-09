#include "Vec2.hpp"
#include <cassert>
#include "PhiMath.hpp"

namespace Phoenix
{
	Vec2& Vec2::operator+=(const Vec2& rhv)
	{
		x += rhv.x;
		y += rhv.y;
		return *this;
	}

	Vec2& Vec2::operator-=(const Vec2& rhv)
	{
		x -= rhv.x;
		y -= rhv.y;
		return *this;
	}

	Vec2& Vec2::operator*=(const Vec2& rhv)
	{
		x *= rhv.x;
		y *= rhv.y;
		return *this;
	}

	Vec2& Vec2::operator/=(const Vec2& rhv)
	{
		x /= rhv.x;
		y /= rhv.y;
		return *this;
	}

	Vec2& Vec2::operator+=(float rhv)
	{
		x += rhv;
		y += rhv;
		return *this;
	}

	Vec2& Vec2::operator-=(float rhv)
	{
		x -= rhv;
		y -= rhv;
		return *this;
	}

	Vec2& Vec2::operator*=(float rhv)
	{
		x *= rhv;
		y *= rhv;
		return *this;
	}

	Vec2& Vec2::operator/=(float rhv)
	{
		assert(rhv != 0);
		x /= rhv;
		y /= rhv;
		return *this;
	}

	bool Vec2::operator==(const Vec2& rhv) const
	{
		return almostEqualRelative(x, rhv.x) && almostEqualRelative(y, rhv.y);
	}

	float Vec2::length() const
	{
		return std::sqrt(x * x + y * y);
	}

	float Vec2::length2() const
	{
		return x * x + y * y;
	}

	float Vec2::distance(const Vec2& rhv) const
	{
		return std::sqrt((x * rhv.x) * (x * rhv.x) - (y * rhv.y) * (y * rhv.y));
	}

	float Vec2::distance2(const Vec2& rhv) const
	{
		return (x * rhv.x) * (x * rhv.x) - (y * rhv.y) * (y * rhv.y);
	}

	Vec2& Vec2::normalize()
	{
		float len = length();
		x /= len;
		y /= len;
		return *this;
	}

	Vec2 Vec2::normalized() const
	{
		Vec2 norm{ *this };
		norm.normalize();
		return norm;
	}

	float Vec2::dot(const Vec2& rhv) const
	{
		return x * rhv.x + y * rhv.y;
	}

	Vec2 Vec2::reflect(const Vec2& toReflect) const
	{
		return (*this) * 2.f * toReflect.dot(*this) - toReflect;
	}

	Vec2 operator+(Vec2 lhv, const Vec2& rhv)
	{
		lhv += rhv;
		return lhv;
	}

	Vec2 operator-(Vec2 lhv, const Vec2& rhv)
	{
		lhv -= rhv;
		return lhv;
	}

	Vec2 operator*(Vec2 lhv, const Vec2& rhv)
	{
		lhv *= rhv;
		return lhv;
	}

	Vec2 operator/(Vec2 lhv, const Vec2& rhv)
	{
		lhv /= rhv;
		return lhv;
	}

	Vec2 operator+(Vec2 lhv, float rhv)
	{
		lhv += rhv;
		return lhv;
	}

	Vec2 operator-(Vec2 lhv, float rhv)
	{
		lhv -= rhv;
		return lhv;
	}

	Vec2 operator*(Vec2 lhv, float rhv)
	{
		lhv *= rhv;
		return lhv;
	}

	Vec2 operator/(Vec2 lhv, float rhv)
	{
		lhv /= rhv;
		return lhv;
	}
}