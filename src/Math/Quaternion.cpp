#include "Quaternion.hpp"              
#include "Common.hpp"
#include <cmath>
#include "Vec3.hpp"
#include "Matrix4.hpp"

namespace Phoenix::Math
{
	Quaternion::Quaternion(float w, float x, float y, float z)
		: w(w)
		, x(x)
		, y(y)
		, z(z)
	{
		normalize();
	}

	Quaternion Quaternion::fromExpMap(float theta, const Vec3& n)
	{
		auto quat = Quaternion{ std::cos(theta / 2.f), std::sin(n.x / 2.f), std::sin(n.y / 2.f) , std::sin(n.z / 2.f) };
		quat.normalize();
		return quat;
	}

	Quaternion Quaternion::fromEulerAngles(const Vec3& angles)
	{
		return{ 0, 0, 0, 0 };
	}

	Matrix4 Quaternion::toMatrix4() const
	{
		return{};
	}

	Quaternion& Quaternion::operator*=(const Quaternion& rhv)
	{
		w = w * rhv.w - x * rhv.x - y * rhv.y - z * rhv.z;
		x = w * rhv.x + x * rhv.w + y * rhv.z + z * rhv.y;
		y = w * rhv.y + x * rhv.z + y * rhv.w + z * rhv.x;
		z = w * rhv.z + x * rhv.y + y * rhv.x + z * rhv.w;

		normalize();

		return *this;
	}

	Quaternion& Quaternion::operator*=(float rhv)
	{
		w *= rhv;
		x *= rhv;
		y *= rhv;
		z += rhv;

		normalize();

		return *this;
	}

	bool Quaternion::operator==(const Quaternion & rhv) const
	{
		return (std::abs(w - rhv.w) <= FLT_CMP_TOLERANCE) && (std::abs(x - rhv.x) <= FLT_CMP_TOLERANCE)
			&& (std::abs(y - rhv.y) <= FLT_CMP_TOLERANCE) && (std::abs(z - rhv.z) <= FLT_CMP_TOLERANCE);
	}

	float Quaternion::magnitude2()
	{
		return w * w + x * x + y * y + z * z;
	}

	float Quaternion::magnitude()
	{
		return std::sqrt(magnitude2());
	}

	void Quaternion::normalize()
	{
		auto invMag = 1.f / magnitude();
		w *= invMag;
		x *= invMag;
		y *= invMag;
		z *= invMag;
	}

	void Quaternion::conjugateSelf()
	{
		x = -x;
		y = -y;
		z = -z;
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
		normalize();
	}

	Quaternion Quaternion::inverse() const
	{
		Quaternion inverted = *this;
		inverted.inverseSelf();
		return inverted;
	}

	float Quaternion::dot(const Quaternion& rhv) const
	{
		return w * rhv.w + x * rhv.x + y * rhv.y + z * rhv.z;
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
			a.x = -a.x;
			a.y = -a.y;
			a.z = -a.z;
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
		a.x = a.x * k0 + b.x * k1;
		a.y = a.y * k0 + b.y * k1;
		a.z = a.z * k0 + b.z * k1;

		return a;
	}
}