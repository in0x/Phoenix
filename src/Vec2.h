#pragma once
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

		Vec2& operator+=(const Vec2& rhv)
		{
			x += rhv.x;
			y += rhv.y;
			return *this;
		}

		Vec2& operator-=(const Vec2& rhv)
		{
			x -= rhv.x;
			y -= rhv.y;
			return *this;
		}

		Vec2& operator*=(const Vec2& rhv)
		{
			x *= rhv.x;
			y *= rhv.y;
			return *this;
		}

		Vec2& operator/=(const Vec2& rhv)
		{
			x /= rhv.x;
			y /= rhv.y;
			return *this;
		}

		Vec2& operator+=(float rhv)
		{
			x += rhv;
			y += rhv;
			return *this;
		}

		Vec2& operator-=(float rhv)
		{
			x -= rhv;
			y -= rhv;
			return *this;
		}

		Vec2& operator*=(float rhv)
		{
			x *= rhv;
			y *= rhv;
			return *this;
		}

		Vec2& operator/=(float rhv)
		{
			assert(rhv != 0);
			x /= rhv;
			y /= rhv;
			return *this;
		}

		bool operator==(const Vec2& rhv) const
		{
			return x == rhv.x && y == rhv.y;
		}

		float length() const
		{
			return std::sqrt(x * x + y * y);
		}

		float length2() const
		{
			return x * x + y * y;
		}

		float distance(const Vec2& rhv) const
		{
			return std::sqrt((x * rhv.x) * (x * rhv.x) - (y * rhv.y) * (y * rhv.y));
		}

		float distance2(const Vec2& rhv) const
		{
			return (x * rhv.x) * (x * rhv.x) - (y * rhv.y) * (y * rhv.y);
		}

		Vec2& normalize()
		{
			float len = length();
			x /= len;
			y /= len;
			return *this;
		}

		float dot(const Vec2& rhv) const
		{
			return x * rhv.x + y * rhv.y;
		}

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

	inline Vec2 Vec2::reflect(const Vec2& rhv) const
	{
		return 2.f * rhv.dot(*this) * (*this) - rhv;
	}

	inline std::ostream& operator<<(std::ostream& out, const Phoenix::Vec2& vec)
	{
		out << "( " << vec.x << " | " << vec.y << " | " << " )";
		return out;
	}
}

