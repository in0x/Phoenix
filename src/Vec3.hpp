#pragma once
#include <iostream>
#include <cmath>
#include <cassert>

namespace Phoenix::Math
{
	class Vec3
	{
	public:
		float x, y, z;

		Vec3() 
			: x(0.f)
			, y(0.f)
			, z(0.f) 
		{}

		Vec3(float f) 
			: x(f)
			, y(f)
			, z(f)
		{}

		Vec3(float _x, float _y, float _z) 
			: x(_x)
			, y(_y)
			, z(_z)
		{}

		float& operator[] (int i)  
		{
			return *(&x + i);
		}

		const float& operator[](int i) const
		{
			return *(&x + i);
		}

		Vec3& operator+=(const Vec3& rhv);
		Vec3& operator-=(const Vec3& rhv);
		Vec3& operator*=(const Vec3& rhv);
		Vec3& operator/=(const Vec3& rhv);

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

	inline const Vec3 operator+(Vec3 lhv, const Vec3& rhv) 
	{
		lhv += rhv;
		return lhv;
	}

	inline const Vec3 operator-(Vec3 lhv, const Vec3& rhv)
	{
		lhv -= rhv;
		return lhv;
	}
	
	inline const Vec3 operator*(Vec3 lhv, const Vec3& rhv)
	{
		lhv *= rhv;
		return lhv;
	}
	
	inline const Vec3 operator/(Vec3 lhv, const Vec3& rhv)
	{
		lhv /= rhv;
		return lhv;
	}

	inline const Vec3 operator+(const Vec3& lhv, float rhv)
	{
		return Vec3{ lhv.x + rhv, lhv.y + rhv, lhv.z + rhv };
	}
	
	inline const Vec3 operator-(const Vec3& lhv, float rhv)
	{
		return Vec3{ lhv.x - rhv, lhv.y - rhv, lhv.z - rhv };
	}
	
	inline const Vec3 operator*(const Vec3& lhv, float rhv)
	{
		return Vec3{ lhv.x * rhv, lhv.y * rhv, lhv.z * rhv };
	}
	
	inline const Vec3 operator/(const Vec3& lhv, float rhv)
	{
		return Vec3{ lhv.x / rhv, lhv.y / rhv, lhv.z / rhv };
	}

	inline std::ostream& operator<<(std::ostream& out, const Vec3& vec)
	{
		out << "( " << vec.x << " | " << vec.y << " | " << vec.z << " )";
		return out;
	}
}

