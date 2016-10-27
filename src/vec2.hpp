#pragma once
#include <iostream>
#include <cmath>
#include <cassert>

namespace phoenix::math
{
	class vec2
	{
	public:
		float x, y;

		vec2()
			: x(0.f)
			, y(0.f)
		{}

		vec2(float _x, float _y)
			: x(_x)
			, y(_y)
		{}

		// Sets out of bounds index to 0.
		float& operator()(int i)
		{
			if (i > 1 || i < 0)
			{
				i = 0;
			}
			return *(&x + i);
		}

		// Sets out of bounds index to 0.
		const float& operator()(int i) const
		{
			if (i > 1 || i < 0)
			{
				i = 0;
			}
			return *(&x + i);
		}

		vec2& operator+=(const vec2& rhv);
		vec2& operator-=(const vec2& rhv);
		vec2& operator*=(const vec2& rhv);
		vec2& operator/=(const vec2& rhv);

		vec2& operator+=(float rhv);
		vec2& operator-=(float rhv);
		vec2& operator*=(float rhv);
		vec2& operator/=(float rhv);

		bool operator==(const vec2& rhv) const;

		float length() const;
		float length2() const;

		float distance(const vec2& rhv) const;
		float distance2(const vec2& rhv) const;

		vec2& normalize();
		float dot(const vec2& rhv) const;
		vec2 reflect(const vec2& rhv) const;
	};

	inline const vec2 operator+(vec2 lhv, const vec2& rhv)
	{
		lhv += rhv;
		return lhv;
	}

	inline const vec2 operator-(vec2 lhv, const vec2& rhv)
	{
		lhv -= rhv;
		return lhv;
	}

	inline const vec2 operator*(vec2 lhv, const vec2& rhv)
	{
		lhv *= rhv;
		return lhv;
	}

	inline const vec2 operator/(vec2 lhv, const vec2& rhv)
	{
		lhv /= rhv;
		return lhv;
	}

	inline const vec2 operator+(const vec2& lhv, float rhv)
	{
		return vec2{ lhv.x + rhv, lhv.y + rhv };
	}

	inline const vec2 operator-(const vec2& lhv, float rhv)
	{
		return vec2{ lhv.x - rhv, lhv.y - rhv };
	}

	inline const vec2 operator*(const vec2& lhv, float rhv)
	{
		return vec2{ lhv.x * rhv, lhv.y * rhv };
	}

	inline const vec2 operator*(float lhv, const vec2 rhv)
	{
		return lhv * rhv;
	}

	inline const vec2 operator/(const vec2& lhv, float rhv)
	{
		return vec2{ lhv.x / rhv, lhv.y / rhv };
	}

	inline std::ostream& operator<<(std::ostream& out, const vec2& vec)
	{
		out << "( " << vec.x << " | " << vec.y << " | " << " )";
		return out;
	}
}

