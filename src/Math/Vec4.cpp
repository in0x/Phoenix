#include "Vec4.hpp"
#include "Common.hpp"
#include "Matrix4.hpp"

namespace Phoenix::Math
{
	Vec4& Vec4::operator+=(const Vec4& rhv)
	{
		x += rhv.x;
		y += rhv.y;
		z += rhv.z;
		w += rhv.w;
		return *this;
	}

	Vec4& Vec4::operator-=(const Vec4& rhv)
	{
		x -= rhv.x;
		y -= rhv.y;
		z -= rhv.z;
		w -= rhv.w;
		return *this;
	}

	Vec4& Vec4::operator*=(const Vec4& rhv)
	{
		x *= rhv.x;
		y *= rhv.y;
		z *= rhv.z;
		w *= rhv.w;
		return *this;
	}

	Vec4& Vec4::operator/=(const Vec4& rhv)
	{
		x /= rhv.x;
		y /= rhv.y;
		z /= rhv.z;
		w /= rhv.w;
		return *this;
	}

	Vec4& Vec4::operator*=(const Matrix4& rhv)
	{
		*this = rhv * (*this);
		return *this;
	}

	Vec4& Vec4::operator+=(float rhv)
	{
		x += rhv;
		y += rhv;
		z += rhv;
		w += rhv;
		return *this;
	}

	Vec4& Vec4::operator-=(float rhv)
	{
		x -= rhv;
		y -= rhv;
		z -= rhv;
		w -= rhv;
		return *this;
	}

	Vec4& Vec4::operator*=(float rhv)
	{
		x *= rhv;
		y *= rhv;
		z *= rhv;
		w *= rhv;
		return *this;
	}

	Vec4& Vec4::operator/=(float rhv)
	{
		assert(rhv != 0);
		x /= rhv;
		y /= rhv;
		z /= rhv;
		w /= rhv;
		return *this;
	}

	Vec4& Vec4::normalize()
	{
		float len = length();
		x /= len;
		y /= len;
		z /= len;
		w /= len;
		return *this;
	}

	bool Vec4::operator==(const Vec4& rhv) const
	{
		return (std::abs(x - rhv.x) <= FLT_CMP_TOLERANCE) && (std::abs(y - rhv.y) <= FLT_CMP_TOLERANCE)
			&& (std::abs(z - rhv.z) <= FLT_CMP_TOLERANCE) && (std::abs(w - rhv.w) <= FLT_CMP_TOLERANCE);
	}

	float Vec4::length() const
	{
		return std::sqrt(x * x + y * y + z * z + w * w);
	}

	float Vec4::length2() const
	{
		return x * x + y * y + z * z + w * w;
	}

	float Vec4::distance(const Vec4& rhv) const
	{
		return std::sqrt((x * rhv.x) * (x * rhv.x) - (y * rhv.y) * (y * rhv.y) - (z * rhv.z) * (z * rhv.z) - (w * rhv.w) * (w * rhv.w));
	}

	float Vec4::distance2(const Vec4& rhv) const
	{
		return (x * rhv.x) * (x * rhv.x) - (y * rhv.y) * (y * rhv.y) - (z * rhv.z) * (z * rhv.z) - (w * rhv.w) * (w * rhv.w);
	}

	float Vec4::dot(const Vec4& rhv) const
	{
		return x * rhv.x + y * rhv.y + z * rhv.z + w * rhv.w;
	}

	Vec4 Vec4::reflect(const Vec4& rhv) const
	{
		return 2.f * rhv.dot(*this) * (*this) - rhv;
	}
}
