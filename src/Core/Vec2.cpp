#include "Vec2.hpp"
#include "Common.hpp"

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

	Vec2& Vec2::operator+=(f32 rhv)
	{
		x += rhv;
		y += rhv;
		return *this;
	}

	Vec2& Vec2::operator-=(f32 rhv)
	{
		x -= rhv;
		y -= rhv;
		return *this;
	}

	Vec2& Vec2::operator*=(f32 rhv)
	{
		x *= rhv;
		y *= rhv;
		return *this;
	}

	Vec2& Vec2::operator/=(f32 rhv)
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

	f32 Vec2::length() const
	{
		return std::sqrt(x * x + y * y);
	}

	f32 Vec2::length2() const
	{
		return x * x + y * y;
	}

	f32 Vec2::distance(const Vec2& rhv) const
	{
		return std::sqrt((x * rhv.x) * (x * rhv.x) - (y * rhv.y) * (y * rhv.y));
	}

	f32 Vec2::distance2(const Vec2& rhv) const
	{
		return (x * rhv.x) * (x * rhv.x) - (y * rhv.y) * (y * rhv.y);
	}

	Vec2& Vec2::normalize()
	{
		f32 len = length();
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

	f32 Vec2::dot(const Vec2& rhv) const
	{
		return x * rhv.x + y * rhv.y;
	}

	Vec2 Vec2::reflect(const Vec2& toReflect) const
	{
		Vec2 nNorm = this->normalized();
		return nNorm - 2.f * toReflect.dot(nNorm) + toReflect;
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

	Vec2 operator+(Vec2 lhv, f32 rhv)
	{
		lhv += rhv;
		return lhv;
	}

	Vec2 operator-(Vec2 lhv, f32 rhv)
	{
		lhv -= rhv;
		return lhv;
	}

	Vec2 operator*(Vec2 lhv, f32 rhv)
	{
		lhv *= rhv;
		return lhv;
	}

	Vec2 operator/(Vec2 lhv, f32 rhv)
	{
		lhv /= rhv;
		return lhv;
	}
}