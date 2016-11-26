#include "Ray.hpp"

namespace Phoenix
{
	Ray::Ray(const Point323D& _origin, const Vec3& _direction)
		: origin(_origin)
		, direction(_direction)
	{
		direction.normalize();
	}


	Point323D Ray::point32At(f32 t) const
	{
		return origin + (direction * t);
	}
}