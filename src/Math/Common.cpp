#include "Common.hpp"

namespace Phoenix
{
	// Uses the fast inverse sqrt trick also used in Quake3.
	float fastInvSqrt(float x)
	{
		float xhalf = 0.5f * x;
		int i = *(int*)&x;            // store floating-point bits in integer
		i = 0x5f3759df - (i >> 1);    // initial guess for Newton's method
		x = *(float*)&i;              // convert new bits into float
		x = x*(1.5f - xhalf*x*x);     // One round of Newton's method
		return x;
	}

}