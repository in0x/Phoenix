#pragma once
#include <algorithm>
#include <limits>

namespace Phoenix
{
	const float VERY_SMALL_FLT = std::numeric_limits<float>::epsilon();

	constexpr float pi()
	{
		return 3.1415926535897932384626433832795f;
	}

	constexpr float degrees(float angleRad)
	{
		return angleRad * (180.f / pi());
	}

	constexpr float radians(float angleDeg)
	{
		return angleDeg * (pi() / 180.f);
	}

	float fastInvSqrt(float x);
	bool almostEqualRelative(float a, float b, float maxRelDif = VERY_SMALL_FLT);
}