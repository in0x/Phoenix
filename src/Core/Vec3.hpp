#pragma once

#include "PhiCoreRequired.hpp"
#include <iostream>
#include <cmath>
#include <cassert>

namespace Phoenix
{
	class Matrix4;

	class Vec3
	{
	public:
		f32 x, y, z;

		Vec3() 
			: x(0.f)
			, y(0.f)
			, z(0.f) 
		{}

		Vec3(f32 _x, f32 _y, f32 _z) 
			: x(_x)
			, y(_y)
			, z(_z)
		{}

		f32& operator()(int32 i)
		{
			return *(&x + i);
		}

		f32 operator()(int32 i) const
		{
			return *(&x + i);
		}

		Vec3 operator -() const
		{
			return{ -x, -y, -z };
		}

		Vec3& operator+=(const Vec3& rhv);
		Vec3& operator-=(const Vec3& rhv);
		Vec3& operator*=(const Vec3& rhv);
		Vec3& operator/=(const Vec3& rhv);

		Vec3& operator*=(const Matrix4& rhv);

		Vec3& operator+=(f32 rhv);
		Vec3& operator-=(f32 rhv);
		Vec3& operator*=(f32 rhv);
		Vec3& operator/=(f32 rhv);

		bool operator==(const Vec3& rhv) const;

		f32 length() const;
		f32 length2() const;

		f32 distance(const Vec3& rhv) const;
		f32 distance2(const Vec3& rhv) const;

		Vec3& normalize();
		
		Vec3 cross(const Vec3& rhv) const;
		f32 dot(const Vec3& rhv) const;
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

	inline Vec3 operator+(const Vec3& lhv, f32 rhv)
	{
		return Vec3{ lhv.x + rhv, lhv.y + rhv, lhv.z + rhv };
	}
	
	inline Vec3 operator-(const Vec3& lhv, f32 rhv)
	{
		return Vec3{ lhv.x - rhv, lhv.y - rhv, lhv.z - rhv };
	}
	
	inline Vec3 operator*(const Vec3& lhv, f32 rhv)
	{
		return Vec3{ lhv.x * rhv, lhv.y * rhv, lhv.z * rhv };
	}

	inline Vec3 operator*(f32 lhv, const Vec3& rhv)
	{
		return rhv * lhv;
	}
	
	inline Vec3 operator/(const Vec3& lhv, f32 rhv)
	{
		return Vec3{ lhv.x / rhv, lhv.y / rhv, lhv.z / rhv };
	}

	inline std::ostream& operator<<(std::ostream& out, const Vec3& vec)
	{
		out << "( " << vec.x << " | " << vec.y << " | " << vec.z << " )";
		return out;
	}
}

