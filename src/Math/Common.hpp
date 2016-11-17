#pragma once

namespace Phoenix::Math
{
	const float FLT_CMP_TOLERANCE = 1.0e-4f;

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
}