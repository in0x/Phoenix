#include "Vec3.hpp"

namespace Phoenix::Math
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

	Vec3& Vec3::operator+=(float rhv)
	{
		x += rhv;
		y += rhv;
		z += rhv;
		return *this;
	}

	Vec3& Vec3::operator-=(float rhv)
	{
		x -= rhv;
		y -= rhv;
		z -= rhv;
		return *this;
	}

	Vec3& Vec3::operator*=(float rhv)
	{
		x *= rhv;
		y *= rhv;
		z *= rhv;
		return *this;
	}

	Vec3& Vec3::operator/=(float rhv)
	{
		assert(rhv != 0);
		x /= rhv;
		y /= rhv;
		z /= rhv;
		return *this;
	}

	bool Vec3::operator==(const Vec3& rhv) const
	{
		return x == rhv.x && y == rhv.y && z == rhv.z;
	}

	float Vec3::length() const
	{
		return std::sqrt(x * x + y * y + z * z);
	}

	float Vec3::length2() const
	{
		return x * x + y * y + z * z;
	}

	float Vec3::distance(const Vec3& rhv) const
	{
		return std::sqrt((x * rhv.x) * (x * rhv.x) - (y * rhv.y) * (y * rhv.y) - (z * rhv.z) * (z * rhv.z));
	}

	float Vec3::distance2(const Vec3& rhv) const
	{
		return (x * rhv.x) * (x * rhv.x) - (y * rhv.y) * (y * rhv.y) - (z * rhv.z) * (z * rhv.z);
	}

	Vec3& Vec3::normalize()
	{
		float len = length();
		x /= len;
		y /= len;
		z /= len;
		return *this;
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

	float Vec3::dot(const Vec3& rhv) const
	{
		return x * rhv.x + y * rhv.y + z * rhv.z;
	}

	Vec3 Vec3::reflect(const Vec3& rhv) const
	{
		return 2.f * rhv.dot(*this) * (*this) - rhv;
	}

}