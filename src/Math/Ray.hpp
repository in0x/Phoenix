#pragma once

#include "Vec3.hpp"
#include "Point3D.hpp"

namespace Phoenix
{
	class Ray
	{
	public:
		Point3D origin;
		Vec3 direction;
		
		Ray(const Point3D& _origin, const Vec3& _direction);

		Point3D pointAt(float t) const;
	};
}