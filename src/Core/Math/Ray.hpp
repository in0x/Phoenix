#pragma once


#include "Vec3.hpp"

namespace Phoenix
{
	class Ray
	{
	public:
		Vec3 origin;
		Vec3 direction;
		
		Ray(const Vec3& _origin, const Vec3& _direction);

		Vec3 pointAt(float t) const;
	};
}