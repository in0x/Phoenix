#include "Ray.hpp"

namespace Phoenix
{
	Ray::Ray(const Vec3& _origin, const Vec3& _direction)
		: origin(_origin)
		, direction(_direction)
	{
		direction.normalize();
	}


	Vec3 Ray::pointAt(f32 t) const
	{
		return origin + (direction * t);
	}
}