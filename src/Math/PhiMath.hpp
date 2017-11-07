#pragma once

#include "Vec2.hpp"
#include "Vec3.hpp"
#include "Vec4.hpp"

#include "Matrix3.hpp"
#include "Matrix4.hpp"

#include "Quaternion.hpp"
#include "EulerAngles.hpp"

#include "Ray.hpp"
#include "Plane.hpp"

namespace Phoenix
{
	// Small float value used to check for < 0 with tolerance for 
	// floating point imprecission. 
	constexpr float VERY_SMALL_FLT = std::numeric_limits<float>::epsilon();

	constexpr float PI = 3.1415926535897932384626433832795f;

	constexpr float degrees(float angleRad)
	{
		return angleRad * (180.f / PI);
	}

	constexpr float radians(float angleDeg)
	{
		return angleDeg * (PI / 180.f);
	}

	float fastInvSqrt(float x);
	
	// If the absolute difference between two floats is within an a small n% of the
	// larger of the two numbers, we can consider them to be equal.
	template<class NumberType>
	bool almostEqualRelative(NumberType a, NumberType b, NumberType maxRelDif = VERY_SMALL_FLT)
	{
		NumberType diff = std::abs(a - b);
		a = std::abs(a);
		b = std::abs(b);

		NumberType larger = std::max(a, b);

		return (diff <= larger * maxRelDif);
	}
	
	Matrix4 lookAtRH(Vec3& cameraPos, Vec3& target, Vec3& up);
	Matrix4 perspectiveRH(float yFOV, float aspect, float near, float far);		
	Matrix4 orthographicRH(float yFOV, float aspect, float near, float far);
}