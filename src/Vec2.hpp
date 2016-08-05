#pragma once
#include <iostream>
#include <cmath>
#include <cassert>

namespace Phoenix::Math
{
	class Vec2
	{
	public:
		float x, y;

		Vec2()
			: x(0.f)
			, y(0.f)
		{}

		Vec2(float f)
			: x(f)
			, y(f)
		{}

		Vec2(float _x, float _y)
			: x(_x)
			, y(_y)
		{}

		float& operator[] (int i)
		{
			return *(&x + i);
		}

		const float& operator[](int i) const
		{
			return *(&x + i);
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
		float dot(const Vec2& rhv) const;
		Vec2 reflect(const Vec2& rhv) const;
	};

	inline const Vec2 operator+(Vec2 lhv, const Vec2& rhv)
	{
		lhv += rhv;
		return lhv;
	}

	inline const Vec2 operator-(Vec2 lhv, const Vec2& rhv)
	{
		lhv -= rhv;
		return lhv;
	}

	inline const Vec2 operator*(Vec2 lhv, const Vec2& rhv)
	{
		lhv *= rhv;
		return lhv;
	}

	inline const Vec2 operator/(Vec2 lhv, const Vec2& rhv)
	{
		lhv /= rhv;
		return lhv;
	}

	inline const Vec2 operator+(const Vec2& lhv, float rhv)
	{
		return Vec2{ lhv.x + rhv, lhv.y + rhv };
	}

	inline const Vec2 operator-(const Vec2& lhv, float rhv)
	{
		return Vec2{ lhv.x - rhv, lhv.y - rhv };
	}

	inline const Vec2 operator*(const Vec2& lhv, float rhv)
	{
		return Vec2{ lhv.x * rhv, lhv.y * rhv };
	}

	inline const Vec2 operator/(const Vec2& lhv, float rhv)
	{
		return Vec2{ lhv.x / rhv, lhv.y / rhv };
	}

	inline std::ostream& operator<<(std::ostream& out, const Phoenix::Vec2& vec)
	{
		out << "( " << vec.x << " | " << vec.y << " | " << " )";
		return out;
	}
}

