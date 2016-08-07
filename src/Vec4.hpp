#pragma once
#include <iostream>
#include <cmath>
#include <cassert>

namespace Phoenix::Math
{
	class Matrix4;

	class Vec4
	{
	public:
		float x, y, z, w;

		Vec4()
			: x(0.f)
			, y(0.f)
			, z(0.f)
			, w(0.f)
		{}

		Vec4(float _x, float _y, float _z, float _w)
			: x(_x)
			, y(_y)
			, z(_z)
			, w (_w)
		{}

		// Sets out of bounds index to 0.
		float& operator()(int i)
		{
			if (i > 3)
			{
				i = 0;
			}
			return *(&x + i);
		}

		// Sets out of bounds index to 0.
		const float& operator()(int i) const
		{
			if (i > 3)
			{
				i = 0;
			}
			return *(&x + i);
		}

		Vec4& operator+=(const Vec4& rhv);
		Vec4& operator-=(const Vec4& rhv);
		Vec4& operator*=(const Vec4& rhv);
		Vec4& operator/=(const Vec4& rhv);

		Vec4& operator*=(const Matrix4& rhv);

		Vec4& operator+=(float rhv);
		Vec4& operator-=(float rhv);
		Vec4& operator*=(float rhv);
		Vec4& operator/=(float rhv);
		
		bool operator==(const Vec4& rhv) const;

		float length() const;
		float length2() const;
	
		float distance(const Vec4& rhv) const;
		float distance2(const Vec4& rhv) const;

		Vec4& normalize();

		float dot(const Vec4& rhv) const;

		Vec4 reflect(const Vec4& rhv) const;
	};

	inline Vec4 operator+(Vec4 lhv, const Vec4& rhv)
	{
		lhv += rhv;
		return lhv;
	}

	inline Vec4 operator-(Vec4 lhv, const Vec4& rhv)
	{
		lhv -= rhv;
		return lhv;
	}

	inline Vec4 operator*(Vec4 lhv, const Vec4& rhv)
	{
		lhv *= rhv;
		return lhv;
	}

	inline Vec4 operator/(Vec4 lhv, const Vec4& rhv)
	{
		lhv /= rhv;
		return lhv;
	}

	inline Vec4 operator+(const Vec4& lhv, float rhv)
	{
		return Vec4{ lhv.x + rhv, lhv.y + rhv, lhv.z + rhv, lhv.w + rhv };
	}

	inline Vec4 operator-(const Vec4& lhv, float rhv)
	{
		return Vec4{ lhv.x - rhv, lhv.y - rhv, lhv.z - rhv, lhv.w - rhv };
	}

	inline Vec4 operator*(const Vec4& lhv, float rhv)
	{
		return Vec4{ lhv.x * rhv, lhv.y * rhv, lhv.z * rhv, lhv.w * rhv };
	}

	inline Vec4 operator*(float lhv, const Vec4& rhv)
	{
		return lhv * rhv;
	}

	inline Vec4 operator/(const Vec4& lhv, float rhv)
	{
		return Vec4{ lhv.x / rhv, lhv.y / rhv, lhv.z / rhv, lhv.w / rhv };
	}
	
	inline std::ostream& operator<<(std::ostream& out, const Vec4& vec)
	{
		out << "( " << vec.x << " | " << vec.y << " | " << vec.z << " | " << vec.w << " )";
		return out;
	}
}

