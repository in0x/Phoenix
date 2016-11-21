#include "Ray.hpp"

namespace Phoenix
{
	Ray::Ray(const Point3D& _origin, const Vec3& _direction)
		: origin(_origin)
		, direction(_direction)
	{
		direction.normalize();
	}


	Point3D Ray::pointAt(float t) const
	{
		return origin + (direction * t);
	}
}