#pragma once

#include "PhiCoreRequired.hpp"
#include <iostream>
#include <cmath>
#include <cassert>

namespace Phoenix
{
	class Matrix4;

	class Vec4
	{
	public:
		f32 x, y, z, w;

		Vec4()
			: x(0.f)
			, y(0.f)
			, z(0.f)
			, w(0.f)
		{}

		Vec4(f32 _x, f32 _y, f32 _z, f32 _w)
			: x(_x)
			, y(_y)
			, z(_z)
			, w (_w)
		{}

		f32& operator()(int32 i)
		{
			return *(&x + i);
		}

		f32 operator()(int32 i) const
		{
			return *(&x + i);
		}

		Vec4 operator -() const
		{
			return{ -x, -y, -z, -w };
		}

		Vec4& operator+=(const Vec4& rhv);
		Vec4& operator-=(const Vec4& rhv);
		Vec4& operator*=(const Vec4& rhv);
		Vec4& operator/=(const Vec4& rhv);

		Vec4& operator*=(const Matrix4& rhv);

		Vec4& operator+=(f32 rhv);
		Vec4& operator-=(f32 rhv);
		Vec4& operator*=(f32 rhv);
		Vec4& operator/=(f32 rhv);
		
		bool operator==(const Vec4& rhv) const;

		f32 length() const;
		f32 length2() const;
	
		f32 distance(const Vec4& rhv) const;
		f32 distance2(const Vec4& rhv) const;

		Vec4& normalize();

		f32 dot(const Vec4& rhv) const;

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

	inline Vec4 operator+(const Vec4& lhv, f32 rhv)
	{
		return Vec4{ lhv.x + rhv, lhv.y + rhv, lhv.z + rhv, lhv.w + rhv };
	}

	inline Vec4 operator-(const Vec4& lhv, f32 rhv)
	{
		return Vec4{ lhv.x - rhv, lhv.y - rhv, lhv.z - rhv, lhv.w - rhv };
	}

	inline Vec4 operator*(const Vec4& lhv, f32 rhv)
	{
		return Vec4{ lhv.x * rhv, lhv.y * rhv, lhv.z * rhv, lhv.w * rhv };
	}

	inline Vec4 operator*(f32 lhv, Vec4 rhv)
	{
		rhv *= lhv;
		return rhv;
	}

	inline Vec4 operator/(const Vec4& lhv, f32 rhv)
	{
		return Vec4{ lhv.x / rhv, lhv.y / rhv, lhv.z / rhv, lhv.w / rhv };
	}
	
	inline std::ostream& operator<<(std::ostream& out, const Vec4& vec)
	{
		out << "( " << vec.x << " | " << vec.y << " | " << vec.z << " | " << vec.w << " )";
		return out;
	}
}

