#include "Vec3.hpp"
#include "Common.hpp"
#include "Matrix4.hpp"

namespace Phoenix
{
	Vec3& Vec3::operator+=(const Vec3& rhv)
	{
		x += rhv.x;
		y += rhv.y;
		z += rhv.z;
		return *this;
	}

	Vec3& Vec3::operator-=(const Vec3& rhv)
	{
		x -= rhv.x;
		y -= rhv.y;
		z -= rhv.z;
		return *this;
	}

	Vec3& Vec3::operator*=(const Vec3& rhv)
	{
		x *= rhv.x;
		y *= rhv.y;
		z *= rhv.z;
		return *this;
	}

	Vec3& Vec3::operator/=(const Vec3& rhv)
	{
		x /= rhv.x;
		y /= rhv.y;
		z /= rhv.z;
		return *this;
	}


	Vec3& Vec3::operator*=(const Matrix4& rhv)
	{
		*this = rhv * (*this);
		return *this;
	}

	Vec3& Vec3::operator+=(f32 rhv)
	{
		x += rhv;
		y += rhv;
		z += rhv;
		return *this;
	}

	Vec3& Vec3::operator-=(f32 rhv)
	{
		x -= rhv;
		y -= rhv;
		z -= rhv;
		return *this;
	}

	Vec3& Vec3::operator*=(f32 rhv)
	{
		x *= rhv;
		y *= rhv;
		z *= rhv;
		return *this;
	}

	Vec3& Vec3::operator/=(f32 rhv)
	{
		assert(rhv != 0);
		x /= rhv;
		y /= rhv;
		z /= rhv;
		return *this;
	}

	bool Vec3::operator==(const Vec3& rhv) const
	{
		return almostEqualRelative(x, rhv.x) && almostEqualRelative(y, rhv.y)
			&& almostEqualRelative(z, rhv.z);
	}

	f32 Vec3::length() const
	{
		return std::sqrt(x * x + y * y + z * z);
	}

	f32 Vec3::length2() const
	{
		return x * x + y * y + z * z;
	}

	f32 Vec3::distance(const Vec3& rhv) const
	{
		return std::sqrt((x * rhv.x) * (x * rhv.x) - (y * rhv.y) * (y * rhv.y) - (z * rhv.z) * (z * rhv.z));
	}

	f32 Vec3::distance2(const Vec3& rhv) const
	{
		return (x * rhv.x) * (x * rhv.x) - (y * rhv.y) * (y * rhv.y) - (z * rhv.z) * (z * rhv.z);
	}

	Vec3& Vec3::normalize()
	{
		f32 len = length();
		x /= len;
		y /= len;
		z /= len;
		return *this;
	}

	Vec3 Vec3::normalized() const
	{
		Vec3 norm{ *this };
		norm.normalize();
		return norm;
	}

	Vec3 Vec3::cross(const Vec3& rhv) const
	{
		return Vec3
		{
			y * rhv.z - z * rhv.y,
			z * rhv.x - x * rhv.z,
			x * rhv.y - y * rhv.x
		};
	}

	f32 Vec3::dot(const Vec3& rhv) const
	{
		return x * rhv.x + y * rhv.y + z * rhv.z;
	}

	Vec3 Vec3::reflect(const Vec3& toReflect) const
	{
		return (*this) * 2.f * toReflect.dot(*this) - toReflect;
	}

	Vec3 operator+(Vec3 lhv, const Vec3& rhv)
	{
		lhv += rhv;
		return lhv;
	}

	Vec3 operator-(Vec3 lhv, const Vec3& rhv)
	{
		lhv -= rhv;
		return lhv;
	}

	Vec3 operator*(Vec3 lhv, const Vec3& rhv)
	{
		lhv *= rhv;
		return lhv;
	}

	Vec3 operator/(Vec3 lhv, const Vec3& rhv)
	{
		lhv /= rhv;
		return lhv;
	}

	Vec3 operator+(Vec3 lhv, f32 rhv)
	{
		lhv += rhv;
		return lhv;
	}

	Vec3 operator-(Vec3 lhv, f32 rhv)
	{
		lhv -= rhv;
		return lhv;
	}

	Vec3 operator*(Vec3 lhv, f32 rhv)
	{
		lhv *= rhv;
		return lhv;
	}

	Vec3 operator/(Vec3 lhv, f32 rhv)
	{
		lhv /= rhv;
		return lhv;
	}
}