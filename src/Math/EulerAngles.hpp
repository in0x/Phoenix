#pragma once
#include "Vec3.hpp"

namespace Phoenix
{
	// Placeholder interface incase the need for a dedicated 
	// Euler angles representation arises later (i.e. to limit
	// the angles to the canonical set).
	class EulerAngles : public Vec3 
	{
	public:
		EulerAngles(float _x, float _y, float _z) : Vec3(_x, _y, _z) 
		{}
	};
}