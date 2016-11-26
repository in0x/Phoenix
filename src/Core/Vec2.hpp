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
		f32 x, y;
		
		Vec2()
			: x(0.f)
			, y(0.f)
		{}

		Vec2(f32 _x, f32 _y)
			: x(_x)
			, y(_y)
		{}

		f32& operator()(int32 i)
		{
			return *(&x + i);
		}

		f32 operator()(int32 i) const
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

		Vec2& operator+=(f32 rhv);
		Vec2& operator-=(f32 rhv);
		Vec2& operator*=(f32 rhv);
		Vec2& operator/=(f32 rhv);

		bool operator==(const Vec2& rhv) const;

		f32 length() const;
		f32 length2() const;

		f32 distance(const Vec2& rhv) const;
		f32 distance2(const Vec2& rhv) const;

		Vec2& normalize();
		f32 dot(const Vec2& rhv) const;
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

	inline const Vec2 operator+(const Vec2& lhv, f32 rhv)
	{
		return Vec2{ lhv.x + rhv, lhv.y + rhv };
	}

	inline const Vec2 operator-(const Vec2& lhv, f32 rhv)
	{
		return Vec2{ lhv.x - rhv, lhv.y - rhv };
	}

	inline const Vec2 operator*(const Vec2& lhv, f32 rhv)
	{
		return Vec2{ lhv.x * rhv, lhv.y * rhv };
	}

	inline const Vec2 operator*(f32 lhv, Vec2 rhv)
	{
		rhv *= lhv;
		return rhv;
	}

	inline const Vec2 operator/(const Vec2& lhv, f32 rhv)
	{
		return Vec2{ lhv.x / rhv, lhv.y / rhv };
	}

	inline std::ostream& operator<<(std::ostream& out, const Vec2& vec)
	{
		out << "( " << vec.x << " | " << vec.y << " | " << " )";
		return out;
	}
}

