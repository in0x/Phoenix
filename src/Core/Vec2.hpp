#pragma once

#include "PhiCoreRequired.hpp"
#include <iostream>
#include <cmath>
#include <cassert>

namespace Phoenix
{
	class Vec2
	{
	public:
		float x, y;
		
		Vec2()
			: x(0.f)
			, y(0.f)
		{}

		Vec2(float _x, float _y)
			: x(_x)
			, y(_y)
		{}

		float& operator()(int i)
		{
			return *(&x + i);
		}

		float operator()(int i) const
		{
			return *(&x + i);
		}

		Vec2 operator -() const
		{
			return{ -x, -y };
		}

		Vec2& operator+=(const Vec2& rhv);
		Vec2& operator-=(const Vec2& rhv);
		Vec2& operator*=(const Vec2& rhv);
		Vec2& operator/=(const Vec2& rhv);

		Vec2& operator+=(float rhv);
		Vec2& operator-=(float rhv);
		Vec2& operator*=(float rhv);
		Vec2& operator/=(float rhv);

		bool operator==(const Vec2& rhv) const;

		float length() const;
		float length2() const;

		float distance(const Vec2& rhv) const;
		float distance2(const Vec2& rhv) const;

		Vec2& normalize();
		Vec2 normalized() const;

		float dot(const Vec2& rhv) const;
		Vec2 reflect(const Vec2& rhv) const;
	};

	Vec2 operator+(Vec2 lhv, const Vec2& rhv);
	Vec2 operator-(Vec2 lhv, const Vec2& rhv);
	Vec2 operator*(Vec2 lhv, const Vec2& rhv);
	Vec2 operator/(Vec2 lhv, const Vec2& rhv);
	Vec2 operator+(Vec2 lhv, float rhv);
	Vec2 operator-(Vec2 lhv, float rhv);
	Vec2 operator*(Vec2 lhv, float rhv);
	Vec2 operator/(Vec2 lhv, float rhv);

	inline std::ostream& operator<<(std::ostream& out, const Vec2& vec)
	{
		out << "( " << vec.x << " | " << vec.y << " | " << " )";
		return out;
	}
}

