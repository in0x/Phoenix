#pragma once

#include "PhiCoreRequired.hpp"
#include "Point3D.hpp"

namespace Phoenix
{
	class Ray
	{
	public:
		Point323D origin;
		Vec3 direction;
		
		Ray(const Point323D& _origin, const Vec3& _direction);

		Point323D point32At(f32 t) const;
	};
}