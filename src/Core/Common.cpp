#include "Common.hpp"

namespace Phoenix
{
	// Uses the fast inverse sqrt trick also used in Quake3.
	f32 fastInvSqrt(f32 x)
	{
		f32 xhalf = 0.5f * x;
		int32 i = *(int32*)&x;            // store f32ing-point32 bits in int32eger
		i = 0x5f3759df - (i >> 1);    // initial guess for Newton's method
		x = *(f32*)&i;              // convert new bits int32o f32
		x = x*(1.5f - xhalf*x*x);     // One round of Newton's method
		return x;
	}

	// If the absolute difference between two f32s is within an a small n% of the
	// larger of the two numbers, we can consider them to be equal.
	bool almostEqualRelative(f32 a, f32 b, f32 maxRelDif)
	{
		f32 diff = std::abs(a - b);
		a = std::abs(a);
		b = std::abs(b);

		f32 larger = std::max(a, b);

		if (diff <= larger * maxRelDif)
			return true;

		return false;
	}
}