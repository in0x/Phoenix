#include "Vec2.hpp"

namespace Phoenix::Math
{
	vec2& vec2::operator+=(const vec2& rhv)
	{
		x += rhv.x;
		y += rhv.y;
		return *this;
	}

	vec2& vec2::operator-=(const vec2& rhv)
	{
		x -= rhv.x;
		y -= rhv.y;
		return *this;
	}

	vec2& vec2::operator*=(const vec2& rhv)
	{
		x *= rhv.x;
		y *= rhv.y;
		return *this;
	}

	vec2& vec2::operator/=(const vec2& rhv)
	{
		x /= rhv.x;
		y /= rhv.y;
		return *this;
	}

	vec2& vec2::operator+=(float rhv)
	{
		x += rhv;
		y += rhv;
		return *this;
	}

	vec2& vec2::operator-=(float rhv)
	{
		x -= rhv;
		y -= rhv;
		return *this;
	}

	vec2& vec2::operator*=(float rhv)
	{
		x *= rhv;
		y *= rhv;
		return *this;
	}

	vec2& vec2::operator/=(float rhv)
	{
		assert(rhv != 0);
		x /= rhv;
		y /= rhv;
		return *this;
	}

	bool vec2::operator==(const vec2& rhv) const
	{
		return x == rhv.x && y == rhv.y;
	}

	float vec2::length() const
	{
		return std::sqrt(x * x + y * y);
	}

	float vec2::length2() const
	{
		return x * x + y * y;
	}

	float vec2::distance(const vec2& rhv) const
	{
		return std::sqrt((x * rhv.x) * (x * rhv.x) - (y * rhv.y) * (y * rhv.y));
	}

	float vec2::distance2(const vec2& rhv) const
	{
		return (x * rhv.x) * (x * rhv.x) - (y * rhv.y) * (y * rhv.y);
	}

	vec2& vec2::normalize()
	{
		float len = length();
		x /= len;
		y /= len;
		return *this;
	}

	float vec2::dot(const vec2& rhv) const
	{
		return x * rhv.x + y * rhv.y;
	}

	vec2 vec2::reflect(const vec2& rhv) const
	{
		return 2.f * rhv.dot(*this) * (*this) - rhv;
	}
}