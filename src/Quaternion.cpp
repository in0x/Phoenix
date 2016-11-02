#pragma once

#include <cmath>

#include "Quaternion.hpp"              
#include "Vec3.hpp"
#include "Matrix4.hpp"

namespace Phoenix::Math
{
	Quaternion::Quaternion(float w, const Vec3& v)
		: w(w)
		, v(v)
	{}

	Quaternion Quaternion::fromExpMap(float theta, const Vec3& n)
	{
		return Quaternion{ std::cos(theta / 2.f), Vec3{std::sin(n.x / 2.f), std::sin(n.y / 2.f) , std::sin(n.z / 2.f) } };
	}

	Quaternion Quaternion::fromEulerAngles(const Vec3& angles)
	{
		return{ 0, Vec3{0,0,0} };
	}

	Matrix4 Quaternion::toMatrix4()
	{
		return{};
	}

	Quaternion& Quaternion::operator*=(const Quaternion& rhv)
	{
		w   = w * rhv.w - v.x * rhv.v.x - v.y * rhv.v.y - v.z * rhv.v.z;
		v.x = w * rhv.v.x + v.x * rhv.w + v.y * rhv.v.z + v.z * rhv.v.y;
		v.y = w * rhv.v.y + v.x * rhv.v.z + v.y * rhv.w + v.z * rhv.v.x;
		v.z = w * rhv.v.z + v.x * rhv.v.y + v.y * rhv.v.x + v.z * rhv.w;
		return *this;
	}

	Quaternion& Quaternion::operator*=(float rhv)
	{
		w *= rhv;
		v *= rhv;
		return *this;
	}

	float Quaternion::magnitude()
	{
		return std::sqrtf(w * w + v.length());
	}

	void Quaternion::conjugateSelf()
	{
		v = -v;
	}

	Quaternion Quaternion::conjugate() const
	{
		Quaternion conj = *this;
		conj.conjugateSelf();
		return conj;
	}

	void Quaternion::inverseSelf()
	{
		conjugateSelf();
		auto mag = magnitude();
		w /= mag;
		v /= mag;
	}

	Quaternion Quaternion::inverse() const
	{
		Quaternion inverted = *this;
		inverted.inverseSelf();
		return inverted;
	}

	float Quaternion::dot(const Quaternion& rhv) const
	{
		return w * rhv.w + v.x * rhv.v.x + v.y * rhv.v.y + v.z * rhv.v.z;
	}


	Quaternion operator*(Quaternion lhv, float rhv)
	{
		lhv *= rhv;
		return lhv;
	}

	Quaternion operator*(Quaternion lhv, const Quaternion& rhv)
	{
		lhv *= rhv;
		return lhv;
	}

	// Geometric interpretation of Slerp (interpolation over points on 4D hypersphere)
	Quaternion slerp(Quaternion a, const Quaternion& b, float t)
	{
		float cosOmega = a.dot(b);

		// If dot product is negative, we invert to calculate the
		// smaller of the two possible rotations.
		if (cosOmega < 0.f)
		{
			a.w = -a.w;
			a.v = -a.v;
		}

		float k0, k1;

		// If dot is close to 0, the two quaternions represent very close
		// orientations, so we just use lerp.
		if (cosOmega > 0.9999f)
		{
			k0 = 1.f - t;
			k1 = t;
		}
		else
		{
			// use pythagorean identity to to get sinOmega
			float sinOmega = std::sqrt(1.f - cosOmega * cosOmega);

			float omega = std::atan2(sinOmega, cosOmega);

			k0 = std::sin((1.f - t) * omega) / omega;
			k1 = std::sin(t * omega) / omega;
		}

		a.w = a.w * k0 + b.w * k1;
		a.v.x = a.v.x * k0 + b.v.x * k1;
		a.v.y = a.v.y * k0 + b.v.y * k1;
		a.v.z = a.v.z * k0 + b.v.z * k1;

		return a;
	}
}