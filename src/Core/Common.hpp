#pragma once
#include <algorithm>
#include <limits>
#include "PhiCoreRequired.hpp"

namespace Phoenix
{
	// Small f32 value used to check for < 0 with tolerance for 
	// floating point imprecission. 
	constexpr f32 VERY_SMALL_FLT = std::numeric_limits<f32>::epsilon();

	constexpr f32 PI = 3.1415926535897932384626433832795f;

	constexpr f32 degrees(f32 angleRad)
	{
		return angleRad * (180.f / PI);
	}

	constexpr f32 radians(f32 angleDeg)
	{
		return angleDeg * (PI / 180.f);
	}

	f32 fastInvSqrt(f32 x);
	bool almostEqualRelative(f32 a, f32 b, f32 maxRelDif = VERY_SMALL_FLT);
}