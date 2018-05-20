#pragma once

namespace Phoenix
{
	class Matrix4;
	class Vec4;

	class Vec3
	{
	public:
		float x, y, z;

		constexpr Vec3() 
			: x(0.f)
			, y(0.f)
			, z(0.f) 
		{}

		constexpr Vec3(float _x, float _y, float _z) 
			: x(_x)
			, y(_y)
			, z(_z)
		{}

		constexpr Vec3(float val)
			: x(val)
			, y(val)
			, z(val)
		{}

		Vec3(const Vec4& xyzw);

		float& operator()(int i)
		{
			return *(&x + i);
		}

		float operator()(int i) const
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
		Vec3 normalized() const;
		
		Vec3 cross(const Vec3& rhv) const;
		float dot(const Vec3& rhv) const;
		Vec3 reflect(const Vec3& toReflect) const;
	};

	Vec3 operator+(Vec3 lhv, const Vec3& rhv);
	Vec3 operator-(Vec3 lhv, const Vec3& rhv);
	Vec3 operator*(Vec3 lhv, const Vec3& rhv);
	Vec3 operator/(Vec3 lhv, const Vec3& rhv);
	Vec3 operator+(Vec3 lhv, float rhv);
	Vec3 operator-(Vec3 lhv, float rhv);
	Vec3 operator*(Vec3 lhv, float rhv);
	Vec3 operator/(Vec3 lhv, float rhv);
}

