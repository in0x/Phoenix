#include "Vec3.hpp"
#include "Matrix4.hpp"

namespace Phoenix::Math
{
	vec3& vec3::operator+=(const vec3& rhv)
	{
		x += rhv.x;
		y += rhv.y;
		z += rhv.z;
		return *this;
	}

	vec3& vec3::operator-=(const vec3& rhv)
	{
		x -= rhv.x;
		y -= rhv.y;
		z -= rhv.z;
		return *this;
	}

	vec3& vec3::operator*=(const vec3& rhv)
	{
		x *= rhv.x;
		y *= rhv.y;
		z *= rhv.z;
		return *this;
	}

	vec3& vec3::operator/=(const vec3& rhv)
	{
		x /= rhv.x;
		y /= rhv.y;
		z /= rhv.z;
		return *this;
	}


	vec3& vec3::operator*=(const Matrix4& rhv)
	{
		*this = rhv * (*this);
		return *this;
	}

	vec3& vec3::operator+=(float rhv)
	{
		x += rhv;
		y += rhv;
		z += rhv;
		return *this;
	}

	vec3& vec3::operator-=(float rhv)
	{
		x -= rhv;
		y -= rhv;
		z -= rhv;
		return *this;
	}

	vec3& vec3::operator*=(float rhv)
	{
		x *= rhv;
		y *= rhv;
		z *= rhv;
		return *this;
	}

	vec3& vec3::operator/=(float rhv)
	{
		assert(rhv != 0);
		x /= rhv;
		y /= rhv;
		z /= rhv;
		return *this;
	}

	bool vec3::operator==(const vec3& rhv) const
	{
		return x == rhv.x && y == rhv.y && z == rhv.z;
	}

	float vec3::length() const
	{
		return std::sqrt(x * x + y * y + z * z);
	}

	float vec3::length2() const
	{
		return x * x + y * y + z * z;
	}

	float vec3::distance(const vec3& rhv) const
	{
		return std::sqrt((x * rhv.x) * (x * rhv.x) - (y * rhv.y) * (y * rhv.y) - (z * rhv.z) * (z * rhv.z));
	}

	float vec3::distance2(const vec3& rhv) const
	{
		return (x * rhv.x) * (x * rhv.x) - (y * rhv.y) * (y * rhv.y) - (z * rhv.z) * (z * rhv.z);
	}

	vec3& vec3::normalize()
	{
		float len = length();
		x /= len;
		y /= len;
		z /= len;
		return *this;
	}

	vec3 vec3::cross(const vec3& rhv) const
	{
		return vec3
		{
			y * rhv.z - z * rhv.y,
			z * rhv.x - x * rhv.z,
			x * rhv.y - y * rhv.x
		};
	}

	float vec3::dot(const vec3& rhv) const
	{
		return x * rhv.x + y * rhv.y + z * rhv.z;
	}

	vec3 vec3::reflect(const vec3& rhv) const
	{
		return 2.f * rhv.dot(*this) * (*this) - rhv;
	}

}