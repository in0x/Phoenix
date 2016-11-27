#pragma once

#include "PhiCoreRequired.hpp"
#include "Vec3.hpp"

namespace Phoenix
{
	class Ray
	{
	public:
		Vec3 origin;
		Vec3 direction;
		
		Ray(const Vec3& _origin, const Vec3& _direction);

		Vec3 pointAt(f32 t) const;
	};
}