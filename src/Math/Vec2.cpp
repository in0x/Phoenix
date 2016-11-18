#include "Vec2.hpp"
#include "Common.hpp"

namespace Phoenix::Math
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
		return (std::abs(x - rhv.x) <= FLT_CMP_TOLERANCE) && (std::abs(y - rhv.y) <= FLT_CMP_TOLERANCE);
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

	float Vec2::dot(const Vec2& rhv) const
	{
		return x * rhv.x + y * rhv.y;
	}

	Vec2 Vec2::reflect(const Vec2& rhv) const
	{
		return 2.f * rhv.dot(*this) * (*this) - rhv;
	}
}