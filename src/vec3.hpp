#pragma once
#include <iostream>
#include <cmath>
#include <cassert>

namespace Phoenix::Math
{
	class Matrix4;

	class vec3
	{
	public:
		float x, y, z;

		vec3() 
			: x(0.f)
			, y(0.f)
			, z(0.f) 
		{}

		vec3(float _x, float _y, float _z) 
			: x(_x)
			, y(_y)
			, z(_z)
		{}

		// Sets out of bounds index to 0.
		float& operator()(int i)
		{
			if (i > 2 || i < 0)
			{
				i = 0;
			}
			return *(&x + i);
		}

		// Sets out of bounds index to 0.
		const float& operator()(int i) const
		{
			if (i > 2 || i < 0)
			{
				i = 0;
			}
			return *(&x + i);
		}

		vec3& operator+=(const vec3& rhv);
		vec3& operator-=(const vec3& rhv);
		vec3& operator*=(const vec3& rhv);
		vec3& operator/=(const vec3& rhv);

		vec3& operator*=(const Matrix4& rhv);

		vec3& operator+=(float rhv);
		vec3& operator-=(float rhv);
		vec3& operator*=(float rhv);
		vec3& operator/=(float rhv);

		bool operator==(const vec3& rhv) const;

		float length() const;
		float length2() const;

		float distance(const vec3& rhv) const;
		float distance2(const vec3& rhv) const;

		vec3& normalize();
		
		vec3 cross(const vec3& rhv) const;
		float dot(const vec3& rhv) const;
		vec3 reflect(const vec3& rhv) const;
	};

	inline vec3 operator+(vec3 lhv, const vec3& rhv) 
	{
		lhv += rhv;
		return lhv;
	}

	inline vec3 operator-(vec3 lhv, const vec3& rhv)
	{
		lhv -= rhv;
		return lhv;
	}
	
	inline vec3 operator*(vec3 lhv, const vec3& rhv)
	{
		lhv *= rhv;
		return lhv;
	}
	
	inline vec3 operator/(vec3 lhv, const vec3& rhv)
	{
		lhv /= rhv;
		return lhv;
	}

	inline vec3 operator+(const vec3& lhv, float rhv)
	{
		return vec3{ lhv.x + rhv, lhv.y + rhv, lhv.z + rhv };
	}
	
	inline vec3 operator-(const vec3& lhv, float rhv)
	{
		return vec3{ lhv.x - rhv, lhv.y - rhv, lhv.z - rhv };
	}
	
	inline vec3 operator*(const vec3& lhv, float rhv)
	{
		return vec3{ lhv.x * rhv, lhv.y * rhv, lhv.z * rhv };
	}

	inline vec3 operator*(float lhv, const vec3& rhv)
	{
		return rhv * lhv;
	}
	
	inline vec3 operator/(const vec3& lhv, float rhv)
	{
		return vec3{ lhv.x / rhv, lhv.y / rhv, lhv.z / rhv };
	}

	inline std::ostream& operator<<(std::ostream& out, const vec3& vec)
	{
		out << "( " << vec.x << " | " << vec.y << " | " << vec.z << " )";
		return out;
	}
}

