#pragma once

#include "../PhiCoreRequired.hpp"

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
	bool almostEqualRelative(float a, float b, float maxRelDif = 0.001f);
}