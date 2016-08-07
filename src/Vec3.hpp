#pragma once
#include <iostream>
#include <cmath>
#include <cassert>

namespace Phoenix::Math
{
	class Matrix4;

	class Vec3
	{
	public:
		float x, y, z;

		Vec3() 
			: x(0.f)
			, y(0.f)
			, z(0.f) 
		{}

		Vec3(float _x, float _y, float _z) 
			: x(_x)
			, y(_y)
			, z(_z)
		{}

		// Sets out of bounds index to 0.
		float& operator()(int i)
		{
			if (i > 2)
			{
				i = 0;
			}
			return *(&x + i);
		}

		// Sets out of bounds index to 0.
		const float& operator()(int i) const
		{
			if (i > 2)
			{
				i = 0;
			}
			return *(&x + i);
		}

		Vec3& operator+=(const Vec3& rhv);
		Vec3& operator-=(const Vec3& rhv);
		Vec3& operator*=(const Vec3& rhv);
		Vec3& operator/=(const Vec3& rhv);

		Vec3& operator*=(const Matrix4& rhv);

		Vec3& operator+=(float rhv);
		Vec3& operator-=(float rhv);
		Vec3& operator*=(float rhv);
		Vec3& operator/=(float rhv);

		bool operator==(const Vec3& rhv) const;

		float length() const;
		float length2() const;

		float distance(const Vec3& rhv) const;
		float distance2(const Vec3& rhv) const;

		Vec3& normalize();
		
		Vec3 cross(const Vec3& rhv) const;
		float dot(const Vec3& rhv) const;
		Vec3 reflect(const Vec3& rhv) const;
	};

	inline Vec3 operator+(Vec3 lhv, const Vec3& rhv) 
	{
		lhv += rhv;
		return lhv;
	}

	inline Vec3 operator-(Vec3 lhv, const Vec3& rhv)
	{
		lhv -= rhv;
		return lhv;
	}
	
	inline Vec3 operator*(Vec3 lhv, const Vec3& rhv)
	{
		lhv *= rhv;
		return lhv;
	}
	
	inline Vec3 operator/(Vec3 lhv, const Vec3& rhv)
	{
		lhv /= rhv;
		return lhv;
	}

	inline Vec3 operator+(const Vec3& lhv, float rhv)
	{
		return Vec3{ lhv.x + rhv, lhv.y + rhv, lhv.z + rhv };
	}
	
	inline Vec3 operator-(const Vec3& lhv, float rhv)
	{
		return Vec3{ lhv.x - rhv, lhv.y - rhv, lhv.z - rhv };
	}
	
	inline Vec3 operator*(const Vec3& lhv, float rhv)
	{
		return Vec3{ lhv.x * rhv, lhv.y * rhv, lhv.z * rhv };
	}

	inline Vec3 operator*(float lhv, const Vec3& rhv)
	{
		return rhv * lhv;
	}
	
	inline Vec3 operator/(const Vec3& lhv, float rhv)
	{
		return Vec3{ lhv.x / rhv, lhv.y / rhv, lhv.z / rhv };
	}

	inline std::ostream& operator<<(std::ostream& out, const Vec3& vec)
	{
		out << "( " << vec.x << " | " << vec.y << " | " << vec.z << " )";
		return out;
	}
}

