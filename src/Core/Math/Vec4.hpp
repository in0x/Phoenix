#pragma once

#include "../PhiCoreRequired.hpp"

namespace Phoenix
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

		float& operator()(int i)
		{
			return *(&x + i);
		}

		float operator()(int i) const
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
		Vec4 normalized() const;

		float dot(const Vec4& rhv) const;

		Vec4 reflect(const Vec4& rhv) const;
	};

	Vec4 operator+(Vec4 lhv, const Vec4& rhv);
	Vec4 operator-(Vec4 lhv, const Vec4& rhv);
	Vec4 operator*(Vec4 lhv, const Vec4& rhv);
	Vec4 operator/(Vec4 lhv, const Vec4& rhv);
	Vec4 operator+(Vec4 lhv, float rhv);
	Vec4 operator-(Vec4 lhv, float rhv);
	Vec4 operator*(Vec4 lhv, float rhv);
	Vec4 operator/(Vec4 lhv, float rhv);
	
	inline std::ostream& operator<<(std::ostream& out, const Vec4& vec)
	{
		out << "( " << vec.x << " | " << vec.y << " | " << vec.z << " | " << vec.w << " )";
		return out;
	}
}

